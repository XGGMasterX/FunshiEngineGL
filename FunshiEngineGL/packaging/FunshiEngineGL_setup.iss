; ============================================================================
;  FunshiEngineGL - Instalador Windows (Inno Setup 6)
; ============================================================================
;  PARA PUBLICAR UNA BETA / ALPHA / DEMO:
;    1. Edita MiVersion y MiCanal abajo.
;    2. Ejecuta HacerInstalador.bat (monta packaging/dist/ y compila este
;       script con ISCC). Alternativa manual: ISCC.exe FunshiEngineGL_setup.iss
;    3. El instalador queda en packaging/instalador/.
;
;  Cada canal genera un archivo distinto (evita confusiones entre builds):
;    Demo  -> FunshiEngineGL-0.5.0-demo-setup.exe
;    Alpha -> FunshiEngineGL-0.5.0-alpha-setup.exe
;    Beta  -> FunshiEngineGL-0.5.0-beta-setup.exe
; ============================================================================

#define MiNombre "FunshiEngineGL"
#define MiVersion "0.5.0"
#define MiCanal "alpha"          ; demo | alpha | beta | rc
#define MiExe "FunshiEngineGL.exe"
; MiEdicion se construye con + (expresion evaluada por ISPP): un literal de
; cadena no expande las {#...} internas y quedarian como texto crudo.
#define MiEdicion MiNombre + " " + MiVersion + " (" + MiCanal + ")"
#define MiId "{{62F2D6B7-8C4E-4A1B-B3D5-9E7A1F0C2B8A}}"

[Setup]
AppId={#MiId}
AppName={#MiEdicion}
AppVersion={#MiVersion}-{#MiCanal}
AppVerName={#MiEdicion}
AppPublisher=FunshiEngineGL
AppComments=Camara y editor de escenas 3D con OpenGL e ImGui
DefaultDirName={autopf}\FunshiEngineGL
DefaultGroupName=FunshiEngineGL
UninstallDisplayIcon={app}\{#MiExe}
Compression=lzma2
SolidCompression=yes
SourceDir=dist
OutputDir=instalador
OutputBaseFilename=FunshiEngineGL-{#MiVersion}-{#MiCanal}-setup
; x64 solamente: el proyecto usa Assimp/Bullet/GLFW de 64 bits (vcpkg x64-windows).
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
; El editor guarda escena y configuracion en C:\MotorGraficoArchivos (raiz).
; Crear ese directorio y subcarpetas requiere permisos de administrador.
PrivilegesRequired=admin
SetupLogging=yes

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Crear acceso directo en el escritorio"; GroupDescription: "Accesos directos:"

[Files]
Source: "{#MiExe}"; DestDir: "{app}"; Flags: ignoreversion
Source: "*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Imagenes\*"; DestDir: "{app}\Imagenes"; Flags: ignoreversion recursesubdirs
Source: "MotorGrafico\*"; DestDir: "{app}\MotorGrafico"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist
Source: "..\..\LICENSE"; DestDir: "{app}\licencia"; Flags: ignoreversion
Source: "..\..\NOTICE"; DestDir: "{app}\licencia"; Flags: ignoreversion
Source: "..\..\THIRD_PARTY_NOTICES.md"; DestDir: "{app}\licencia"; Flags: ignoreversion

; ============================================================================
; Estructura del proyecto del usuario (donde el motor guarda escena + config al
; salir y donde arrastra assets). Se pre-crea vacia para que el primer arranque
; no falle al escribir SceneBBDDObjetos.txt ni los ObjectN#.db (SceneSerializer
; NO crea directorios por su cuenta).
; ============================================================================
[Dirs]
Name: "{app}\MotorGrafico"
Name: "{app}\MotorGrafico\Modelos"
Name: "{app}\MotorGrafico\Texturas"
Name: "{app}\MotorGrafico\Imagenes"
; Carpetas de la "raiz del motor" en Windows (ver EditorConfig.cpp):
;   - Escena y configuracion -> C:/MotorGraficoArchivos/...
Name: "C:\MotorGraficoArchivos\Binarios\Scene"
Name: "C:\MotorGraficoArchivos\Binarios"
Name: "C:\MotorGraficoArchivos\Modelos"
Name: "C:\MotorGraficoArchivos\Texturas"
Name: "C:\MotorGraficoArchivos\Imagenes"

[Icons]
Name: "{group}\{#MiNombre} {#MiVersion} ({#MiCanal})"; Filename: "{app}\{#MiExe}"; WorkingDir: "{app}"
Name: "{group}\Carpeta del proyecto (MotorGrafico)"; Filename: "C:\MotorGraficoArchivos"; WorkingDir: "{app}"
Name: "{autodesktop}\{#MiNombre} {#MiVersion} ({#MiCanal})"; Filename: "{app}\{#MiExe}"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MiExe}"; Description: "Ejecutar {#MiNombre} ahora"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent