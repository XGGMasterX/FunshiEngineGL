/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#include "BackendJava.h"

#include <jni.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "../ScriptGameObject.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifndef FUNSHI_LIBJVM_DEFAULT
#define FUNSHI_LIBJVM_DEFAULT ""
#endif
#ifndef FUNSHI_JAVAC_DEFAULT
#define FUNSHI_JAVAC_DEFAULT "javac"
#endif

namespace fs = std::filesystem;

namespace {

// --- SDK Java embebido ------------------------------------------------------
// Se escribe en el cache y se compila junto con los scripts del usuario. Sin
// paquete para simplificar la classpath y el template generado por la GUI.
const char* SRC_COMPORTAMIENTO =
    "public interface Comportamiento {\n"
    "    void iniciar(long objeto);\n"
    "    void actualizar(long objeto, double deltaTime);\n"
    "    default void detener(long objeto) {}\n"
    "}\n";

const char* SRC_NATIVO =
    "public final class Nativo {\n"
    "    public static native String nombre(long objeto);\n"
    "    public static native float  posicionX(long objeto);\n"
    "    public static native float  posicionY(long objeto);\n"
    "    public static native float  posicionZ(long objeto);\n"
    "    public static native void fijarPosicion(long objeto, float x, float y, float z);\n"
    "    public static native void fijarEscala(long objeto, float x, float y, float z);\n"
    "    public static native void fijarRotacion(long objeto, float angulo, float x, float y, float z);\n"
    "    public static native void imprimir(String texto);\n"
    "    private Nativo() {}\n"
    "}\n";

// --- Bootstrap dinamico del JVM --------------------------------------------
struct Jvm {
    void* biblioteca = nullptr;
    JavaVM* jvm = nullptr;
    JNIEnv* env = nullptr;
    std::string error;
};
Jvm& jvm() {
    static Jvm instancia;
    return instancia;
}

typedef jint(JNICALL* FnCrearJavaVM)(JavaVM**, void**, void*);

std::string directorioCache() {
    std::error_code ec;
    fs::path base;
#if defined(_WIN32)
    const char* tmp = std::getenv("TEMP");
    if (tmp && *tmp) base = tmp;
    else base = ".";
#else
    const char* tmp = std::getenv("TMPDIR");
    if (tmp && *tmp) base = tmp;
    else base = "/tmp";
#endif
    return (base / "funshi_scripts" / "java").string();
}

std::string rutaLibjvm() {
    const char* explicito = std::getenv("FUNSHI_LIBJVM");
    if (explicito && *explicito) return explicito;

    const char* home = std::getenv("JAVA_HOME");
    if (home && *home) {
#if defined(_WIN32)
        fs::path p = fs::path(home) / "bin" / "server" / "jvm.dll";
#elif defined(__APPLE__)
        fs::path p = fs::path(home) / "lib" / "server" / "libjvm.dylib";
#else
        fs::path p = fs::path(home) / "lib" / "server" / "libjvm.so";
#endif
        std::error_code ec;
        if (fs::exists(p, ec)) return p.string();
    }

    // Ruta con la que se configuro el build (FindJNI), si existe.
    std::error_code ec;
    if (std::string(FUNSHI_LIBJVM_DEFAULT).size() > 0 &&
        fs::exists(FUNSHI_LIBJVM_DEFAULT, ec))
        return FUNSHI_LIBJVM_DEFAULT;

    // Busqueda generica en instalaciones tipicas de Linux.
#if !defined(_WIN32)
    for (const char* base : {"/usr/lib/jvm/default-java",
                             "/usr/lib/jvm/java-1.21.0-openjdk-amd64",
                             "/usr/lib/jvm/java-17-openjdk-amd64",
                             "/usr/lib/jvm/java-11-openjdk-amd64"}) {
        fs::path p = fs::path(base) / "lib" / "server" / "libjvm.so";
        if (fs::exists(p, ec)) return p.string();
    }
    for (const fs::directory_entry& entrada :
         fs::directory_iterator("/usr/lib/jvm", ec)) {
        fs::path p = entrada.path() / "lib" / "server" / "libjvm.so";
        if (fs::exists(p, ec)) return p.string();
    }
#endif
    return "";
}

bool arrancarJvm(const std::string& classesDir, std::string& error) {
    if (jvm().jvm) {
        return true; // ya arrancado: la classpath es compartida
    }

    const std::string ruta = rutaLibjvm();
    if (ruta.empty()) {
        error = "No se encontro libjvm (defini JAVA_HOME o FUNSHI_LIBJVM) para "
                "ejecutar scripts Java.";
        return false;
    }

    void* lib = nullptr;
#if defined(_WIN32)
    lib = LoadLibraryA(ruta.c_str());
#else
    lib = dlopen(ruta.c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif
    if (!lib) {
        error = "No se pudo cargar el JVM: " + ruta;
        return false;
    }

    FnCrearJavaVM crear = reinterpret_cast<FnCrearJavaVM>(
#if defined(_WIN32)
        GetProcAddress(static_cast<HMODULE>(lib), "JNI_CreateJavaVM")
#else
        dlsym(lib, "JNI_CreateJavaVM")
#endif
    );
    if (!crear) {
        error = "libjvm no exporta JNI_CreateJavaVM: " + ruta;
        return false;
    }

    const std::string classpath =
        std::string("-Djava.class.path=") + classesDir;
    JavaVMOption opciones[1];
    opciones[0].optionString = const_cast<char*>(classpath.c_str());

    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_8;
    args.nOptions = 1;
    args.options = opciones;
    args.ignoreUnrecognized = JNI_TRUE;

    JNIEnv* env = nullptr;
    JavaVM* maquina = nullptr;
    jint rc = crear(&maquina, reinterpret_cast<void**>(&env), &args);
    if (rc != JNI_OK || !maquina) {
        error = "JNI_CreateJavaVM fallo (codigo " + std::to_string(rc) + ")";
        return false;
    }

    jvm().biblioteca = lib;
    jvm().jvm = maquina;
    jvm().env = env;
    jvm().error.clear();
    return true;
}

JNIEnv* entorno() { return jvm().env; }

std::string javacExe() {
    const char* env = std::getenv("JAVAC");
    if (env && *env) return env;
    return FUNSHI_JAVAC_DEFAULT;
}

std::string firmarCaracter(const std::string& tipo) {
    if (tipo == "int") return "I";
    if (tipo == "float") return "F";
    if (tipo == "double") return "D";
    if (tipo == "boolean") return "Z";
    if (tipo == "java.lang.String") return "Ljava/lang/String;";
    return "";
}

ReflejoScripts::TagTipo etiquetaDeTipo(const std::string& tipo) {
    using ReflejoScripts::TagTipo;
    if (tipo == "int") return TagTipo::Entero;
    if (tipo == "float") return TagTipo::Flotante;
    if (tipo == "double") return TagTipo::Doble;
    if (tipo == "boolean") return TagTipo::Booleano;
    return TagTipo::Texto; // java.lang.String
}

struct DatosJava {
    jclass clase = nullptr;
    jmethodID iniciar = nullptr;
    jmethodID actualizar = nullptr;
    jmethodID detener = nullptr;
    std::map<std::string, jfieldID> campos; // nombre -> jfieldID
};

std::string leerArchivo(const std::string& ruta) {
    std::ifstream f(ruta);
    return std::string((std::istreambuf_iterator<char>(f)),
                       std::istreambuf_iterator<char>());
}

void escribirSiCambia(const std::string& ruta, const std::string& contenido) {
    std::error_code ec;
    if (fs::exists(ruta, ec) && leerArchivo(ruta) == contenido) return;
    fs::create_directories(fs::path(ruta).parent_path(), ec);
    std::ofstream f(ruta);
    f << contenido;
}

} // namespace

// --- Nativos expuestos a Java (Nativo.xxx) ----------------------------------
namespace {
jlong comoHandle(jlong objeto) { return objeto; }

jstring nativoNombre(JNIEnv* env, jclass, jlong objeto) {
    const char* n = MotorScript::tablaApi()->nombre(
        reinterpret_cast<void*>(comoHandle(objeto)));
    return env->NewStringUTF(n ? n : "");
}
jfloat nativoPosicionX(JNIEnv*, jclass, jlong o) {
    return MotorScript::tablaApi()->posicionX(reinterpret_cast<void*>(o));
}
jfloat nativoPosicionY(JNIEnv*, jclass, jlong o) {
    return MotorScript::tablaApi()->posicionY(reinterpret_cast<void*>(o));
}
jfloat nativoPosicionZ(JNIEnv*, jclass, jlong o) {
    return MotorScript::tablaApi()->posicionZ(reinterpret_cast<void*>(o));
}
void nativoFijarPosicion(JNIEnv*, jclass, jlong o, jfloat x, jfloat y, jfloat z) {
    MotorScript::tablaApi()->fijarPosicion(reinterpret_cast<void*>(o), x, y, z);
}
void nativoFijarEscala(JNIEnv*, jclass, jlong o, jfloat x, jfloat y, jfloat z) {
    MotorScript::tablaApi()->fijarEscala(reinterpret_cast<void*>(o), x, y, z);
}
void nativoFijarRotacion(JNIEnv*, jclass, jlong o, jfloat a, jfloat x, jfloat y,
                         jfloat z) {
    MotorScript::tablaApi()->fijarRotacionEjes(reinterpret_cast<void*>(o), a, x,
                                               y, z);
}
void nativoImprimir(JNIEnv* env, jclass, jstring texto) {
    if (!texto) return;
    const char* utf = env->GetStringUTFChars(texto, nullptr);
    if (utf) {
        MotorScript::tablaApi()->imprimirConsola(utf);
        env->ReleaseStringUTFChars(texto, utf);
    }
}

bool registrarNativos(std::string& error) {
    JNIEnv* env = entorno();
    jclass nativo = env->FindClass("Nativo");
    if (!nativo) {
        env->ExceptionClear();
        error = "No se encontro la clase Nativo en la classpath.";
        return false;
    }
    JNINativeMethod metodos[] = {
        {const_cast<char*>("nombre"), const_cast<char*>("(J)Ljava/lang/String;"),
         reinterpret_cast<void*>(&nativoNombre)},
        {const_cast<char*>("posicionX"), const_cast<char*>("(J)F"),
         reinterpret_cast<void*>(&nativoPosicionX)},
        {const_cast<char*>("posicionY"), const_cast<char*>("(J)F"),
         reinterpret_cast<void*>(&nativoPosicionY)},
        {const_cast<char*>("posicionZ"), const_cast<char*>("(J)F"),
         reinterpret_cast<void*>(&nativoPosicionZ)},
        {const_cast<char*>("fijarPosicion"), const_cast<char*>("(JFFF)V"),
         reinterpret_cast<void*>(&nativoFijarPosicion)},
        {const_cast<char*>("fijarEscala"), const_cast<char*>("(JFFF)V"),
         reinterpret_cast<void*>(&nativoFijarEscala)},
        {const_cast<char*>("fijarRotacion"), const_cast<char*>("(JFFFF)V"),
         reinterpret_cast<void*>(&nativoFijarRotacion)},
        {const_cast<char*>("imprimir"), const_cast<char*>("(Ljava/lang/String;)V"),
         reinterpret_cast<void*>(&nativoImprimir)},
    };
    if (env->RegisterNatives(nativo, metodos,
                             sizeof(metodos) / sizeof(metodos[0])) != JNI_OK) {
        error = "RegisterNatives de Nativo fallo.";
        return false;
    }
    return true;
}
} // namespace

const char* BackendJava::lenguaje() const { return "java"; }

bool BackendJava::compilarYCargar(const std::string& fuente,
                                  const std::string& nombreClase,
                                  ComportamientoCargado& salida,
                                  std::string& error) {
    std::error_code ec;
    if (!fs::exists(fuente, ec)) {
        error = "El fuente Java no existe:\n" + fuente;
        return false;
    }

    const std::string cache = directorioCache();
    const std::string clasesDir = (fs::path(cache) / "clases").string();
    const std::string sdkDir = (fs::path(cache) / "sdk").string();
    const std::string logPath = (fs::path(cache) / "javac.log").string();
    fs::create_directories(clasesDir, ec);
    escribirSiCambia((fs::path(sdkDir) / "Comportamiento.java").string(),
                     SRC_COMPORTAMIENTO);
    escribirSiCambia((fs::path(sdkDir) / "Nativo.java").string(), SRC_NATIVO);

    auto mtime = [](const std::string& r) {
        std::error_code e;
        auto t = fs::last_write_time(r, e);
        if (e) return std::string();
        return std::to_string(t.time_since_epoch().count());
    };

    const std::string claseCompilada =
        (fs::path(clasesDir) / (nombreClase + ".class")).string();
    const std::string mtimeFuente = mtime(fuente);
    const bool recompilar = !fs::exists(claseCompilada, ec) ||
                            salida.mtimeFuente != mtimeFuente;
    if (recompilar) {
        const std::string javac = javacExe();
        const std::string sdkComportamiento =
            (fs::path(sdkDir) / "Comportamiento.java").string();
        const std::string sdkNativo = (fs::path(sdkDir) / "Nativo.java").string();
        const std::string cmd =
            "\"" + javac + "\" -d \"" + clasesDir + "\" -cp \"" + clasesDir +
            "\" \"" + sdkComportamiento + "\" \"" + sdkNativo + "\" \"" +
            fuente + "\" > \"" + logPath + "\" 2>&1";
        int rc = std::system(cmd.c_str());
        if (rc != 0) {
            error = "Error al compilar el script Java:\n" + leerArchivo(logPath);
            return false;
        }
    }

    if (!arrancarJvm(clasesDir, error)) return false;
    if (!registrarNativos(error)) return false;

    JNIEnv* env = entorno();
    jclass clase = env->FindClass(nombreClase.c_str());
    if (!clase) {
        env->ExceptionClear();
        error = "No se encontro la clase Java '" + nombreClase +
                "' (¿el nombre de la clase coincide con el archivo?).";
        return false;
    }
    jmethodID constructor = env->GetMethodID(clase, "<init>", "()V");
    if (!constructor) {
        env->ExceptionClear();
        error = "La clase Java '" + nombreClase + "' no tiene constructor vacio.";
        return false;
    }
    jobject objeto = env->NewObject(clase, constructor);
    if (!objeto) {
        env->ExceptionClear();
        error = "No se pudo instanciar la clase Java '" + nombreClase + "'.";
        return false;
    }

    auto* datos = new DatosJava();
    datos->clase = static_cast<jclass>(env->NewGlobalRef(clase));
    datos->iniciar = env->GetMethodID(clase, "iniciar", "(J)V");
    datos->actualizar = env->GetMethodID(clase, "actualizar", "(JD)V");
    datos->detener = env->GetMethodID(clase, "detener", "(J)V");

    // Reflexion de los campos publicos (no estaticos) -> SerializeField.
    std::vector<ReflejoScripts::DefCampo> campos;
    jmethodID obtenerCampos = env->GetMethodID(
        env->FindClass("java/lang/Class"), "getFields",
        "()[Ljava/lang/reflect/Field;");
    jobjectArray arreglo = static_cast<jobjectArray>(
        env->CallObjectMethod(clase, obtenerCampos));
    if (env->ExceptionCheck()) env->ExceptionClear();
    jclass claseField = env->FindClass("java/lang/reflect/Field");
    jclass claseModifier = env->FindClass("java/lang/reflect/Modifier");
    jmethodID getName = env->GetMethodID(claseField, "getName",
                                         "()Ljava/lang/String;");
    jmethodID getType = env->GetMethodID(claseField, "getType",
                                         "()Ljava/lang/Class;");
    jmethodID getMods = env->GetMethodID(claseField, "getModifiers", "()I");
    jmethodID esEstatico =
        env->GetStaticMethodID(claseModifier, "isStatic", "(I)Z");
    jmethodID nombreClaseJava =
        env->GetMethodID(env->FindClass("java/lang/Class"), "getName",
                         "()Ljava/lang/String;");

    const jsize total = arreglo ? env->GetArrayLength(arreglo) : 0;
    for (jsize i = 0; i < total; ++i) {
        jobject campo =
            env->GetObjectArrayElement(static_cast<jobjectArray>(arreglo), i);
        jint mods = env->CallIntMethod(campo, getMods);
        if (env->CallBooleanMethod(claseModifier, esEstatico, mods)) continue;

        jstring jnombre =
            static_cast<jstring>(env->CallObjectMethod(campo, getName));
        jclass tipo = static_cast<jclass>(env->CallObjectMethod(campo, getType));
        jstring jtipo =
            static_cast<jstring>(env->CallObjectMethod(tipo, nombreClaseJava));
        const char* nombreUtf = env->GetStringUTFChars(jnombre, nullptr);
        const char* tipoUtf = env->GetStringUTFChars(jtipo, nullptr);
        const std::string nombre = nombreUtf ? nombreUtf : "";
        const std::string tipoNombre = tipoUtf ? tipoUtf : "";
        env->ReleaseStringUTFChars(jnombre, nombreUtf);
        env->ReleaseStringUTFChars(jtipo, tipoUtf);

        const std::string firma = firmarCaracter(tipoNombre);
        if (firma.empty()) continue; // tipo no soportado: se ignora

        jfieldID id = env->GetFieldID(clase, nombre.c_str(), firma.c_str());
        if (!id) {
            env->ExceptionClear();
            continue;
        }
        ReflejoScripts::DefCampo def;
        def.nombre = nombre;
        def.tag = etiquetaDeTipo(tipoNombre);
        campos.push_back(std::move(def));
        datos->campos[nombre] = id;
    }

    salida.fuente = fuente;
    salida.lenguaje = "java";
    salida.artefacto = clasesDir;
    salida.manejador = jvm().jvm;
    salida.instancia = env->NewGlobalRef(objeto);
    salida.datos = datos;
    salida.campos = std::move(campos);
    salida.mtimeFuente = mtimeFuente;
    salida.cargado = true;
    error.clear();
    return true;
}

void BackendJava::descargar(ComportamientoCargado& comportamiento) {
    descargar(comportamiento, nullptr);
}

void BackendJava::descargar(ComportamientoCargado& comportamiento,
                            GameObject* owner) {
    if (!comportamiento.cargado) return;
    JNIEnv* env = entorno();
    if (env) {
        if (owner && comportamiento.instancia) {
            auto* datos = static_cast<DatosJava*>(comportamiento.datos);
            if (datos && datos->detener)
                env->CallVoidMethod(
                    static_cast<jobject>(comportamiento.instancia),
                    datos->detener, static_cast<jlong>(
                                        reinterpret_cast<intptr_t>(owner)));
        }
        if (comportamiento.instancia)
            env->DeleteGlobalRef(
                static_cast<jobject>(comportamiento.instancia));
        auto* datos = static_cast<DatosJava*>(comportamiento.datos);
        if (datos) {
            if (datos->clase) env->DeleteGlobalRef(datos->clase);
            delete datos;
        }
    }
    comportamiento = ComportamientoCargado{};
}

void BackendJava::llamarInicio(ComportamientoCargado& comportamiento,
                               GameObject* owner) {
    if (!comportamiento.valido()) return;
    auto* datos = static_cast<DatosJava*>(comportamiento.datos);
    if (!datos || !datos->iniciar) return;
    entorno()->CallVoidMethod(
        static_cast<jobject>(comportamiento.instancia), datos->iniciar,
        static_cast<jlong>(reinterpret_cast<intptr_t>(owner)));
}

void BackendJava::llamarActualizar(ComportamientoCargado& comportamiento,
                                   GameObject* owner, float deltaTime) {
    if (!comportamiento.valido()) return;
    auto* datos = static_cast<DatosJava*>(comportamiento.datos);
    if (!datos || !datos->actualizar) return;
    entorno()->CallVoidMethod(
        static_cast<jobject>(comportamiento.instancia), datos->actualizar,
        static_cast<jlong>(reinterpret_cast<intptr_t>(owner)),
        static_cast<jdouble>(deltaTime));
}

void BackendJava::llamarDetener(ComportamientoCargado& comportamiento,
                                GameObject* owner) {
    if (!comportamiento.valido()) return;
    auto* datos = static_cast<DatosJava*>(comportamiento.datos);
    if (!datos || !datos->detener) return;
    entorno()->CallVoidMethod(
        static_cast<jobject>(comportamiento.instancia), datos->detener,
        static_cast<jlong>(reinterpret_cast<intptr_t>(owner)));
}

void BackendJava::inyectar(
    ComportamientoCargado& comportamiento,
    const std::vector<ReflejoScripts::ValorCampo>& valores) {
    if (!comportamiento.valido()) return;
    JNIEnv* env = entorno();
    auto* datos = static_cast<DatosJava*>(comportamiento.datos);
    jobject objeto = static_cast<jobject>(comportamiento.instancia);
    for (const auto& def : comportamiento.campos) {
        const ReflejoScripts::ValorCampo* valor = nullptr;
        for (const auto& v : valores)
            if (v.nombre == def.nombre) {
                valor = &v;
                break;
            }
        if (!valor) continue;
        auto it = datos->campos.find(def.nombre);
        if (it == datos->campos.end()) continue;
        jfieldID id = it->second;
        using ReflejoScripts::TagTipo;
        switch (def.tag) {
        case TagTipo::Entero:
            env->SetIntField(objeto, id, valor->como<int>());
            break;
        case TagTipo::Flotante:
            env->SetFloatField(objeto, id, valor->como<float>());
            break;
        case TagTipo::Doble:
            env->SetDoubleField(objeto, id, valor->como<double>());
            break;
        case TagTipo::Booleano:
            env->SetBooleanField(objeto, id, valor->como<bool>());
            break;
        case TagTipo::Texto: {
            jstring s = env->NewStringUTF(valor->como<std::string>().c_str());
            env->SetObjectField(objeto, id, s);
            break;
        }
        default:
            break;
        }
    }
}

std::vector<ReflejoScripts::ValorCampo> BackendJava::extraer(
    ComportamientoCargado& comportamiento) {
    std::vector<ReflejoScripts::ValorCampo> valores;
    if (!comportamiento.valido()) return valores;
    JNIEnv* env = entorno();
    auto* datos = static_cast<DatosJava*>(comportamiento.datos);
    jobject objeto = static_cast<jobject>(comportamiento.instancia);
    for (const auto& def : comportamiento.campos) {
        auto it = datos->campos.find(def.nombre);
        if (it == datos->campos.end()) continue;
        jfieldID id = it->second;
        ReflejoScripts::ValorCampo valor;
        valor.nombre = def.nombre;
        valor.tag = def.tag;
        using ReflejoScripts::TagTipo;
        switch (def.tag) {
        case TagTipo::Entero:
            valor.contenido = env->GetIntField(objeto, id);
            break;
        case TagTipo::Flotante:
            valor.contenido = env->GetFloatField(objeto, id);
            break;
        case TagTipo::Doble:
            valor.contenido = env->GetDoubleField(objeto, id);
            break;
        case TagTipo::Booleano:
            valor.contenido = static_cast<bool>(env->GetBooleanField(objeto, id));
            break;
        case TagTipo::Texto: {
            jstring s = static_cast<jstring>(env->GetObjectField(objeto, id));
            if (s) {
                const char* utf = env->GetStringUTFChars(s, nullptr);
                valor.contenido = std::string(utf ? utf : "");
                env->ReleaseStringUTFChars(s, utf);
            } else {
                valor.contenido = std::string();
            }
            break;
        }
        default:
            break;
        }
        valores.push_back(std::move(valor));
    }
    return valores;
}