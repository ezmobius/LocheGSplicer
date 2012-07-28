;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;             Installer script for LocheGSplicer              ;;;;
;;;;                                                             ;;;;
;;;;         To generate the installer, download NSIS            ;;;;
;;;;         (http://nsis.sourceforge.net/Download)              ;;;;
;;;;         and compile using this NSI script.                  ;;;;
;;;;                                                             ;;;;
;;;;     Uses environment variables to find external files       ;;;;
;;;;      QT_DIR - Root Qt SDK folder.                           ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                 Paths to assets and binaries                ;;;;
;;;; - Edit these paths if you have custom locations for files   ;;;;
!define PROJECT_BIN_PATH      "..\build\bin"
!define PROJECT_CONTEXT_PATH  "..\data"
!define PROJECT_TEST_PATH     "..\test"

!define QT_PATH               "$%QT_DIR%\bin"
;;;;                                                             ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                            Setup                            ;;;;
;!define DEBUG_INSTALL
!define VS2008

;!define /date NOW "%m-%d-%Y_%H%M%S"

!ifndef VERSION
  !define VERSION "Beta-001"
!endif

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "LocheGSplicer"
!define PRODUCT_VERSION ${VERSION}
!define PRODUCT_DIR_REGKEY "Software\LocheGSplicer"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

!ifdef DEBUG_INSTALL
	!define CONFIG_FLAG "d"
	!define CONFIG_FLAG2 "_d"
	!define CONFIG_NAME "Debug"
!else
	!define CONFIG_FLAG ""
	!define CONFIG_FLAG2 ""
	!define CONFIG_NAME ""
!endif

!ifdef OUTFILE
  OutFile "${OUTFILE}"
!else
  OutFile LocheGSplicer${CONFIG_NAME}-${VERSION}-setup.exe
!endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                              UI                             ;;;;
; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
;!insertmacro MUI_PAGE_LICENSE "c:\path\to\licence\YourSoftwareLicence.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "LocheGSplicer"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\LocheGSplicer.bat"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "C:\LocheGSplicer"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                    Application Binaries                     ;;;;
Section "Application" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer

  ; Batch file
  File "LocheGSplicer${CONFIG_NAME}.bat"
  File "..\README.txt"

  ; Application binaries
  SetOutPath "$INSTDIR\bin"
  File "${PROJECT_BIN_PATH}\LocheGSplicer${CONFIG_FLAG}.exe"
  
  ; Application dependencies.
  SetOutPath "$INSTDIR\bin"
  File "${QT_PATH}\QtCore${CONFIG_FLAG}4.dll"
  File "${QT_PATH}\QtGui${CONFIG_FLAG}4.dll"
  File "${QT_PATH}\QtOpenGL${CONFIG_FLAG}4.dll"

  ; PDB Files for debug.
!ifdef DEBUG_INSTALL
  SetOutPath "$INSTDIR\bin"
  File "${PROJECT_BIN_PATH}\LocheGSplicer${DEBUG_FLAG}.pdb"
!endif
SectionEnd
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                           Assets                            ;;;;
Section "Assets" SEC02
  ; Config
  SetOutPath "$INSTDIR\data\config"
  ;File "${PROJECT_CONTEXT_PATH}\config\config.ini"
  
  SetOutPath "$INSTDIR\test"
  File "${PROJECT_TEST_PATH}\Simple.KISS.gcode"
  File "${PROJECT_TEST_PATH}\Simple.Slic3r.gcode"
SectionEnd

Section "Shortcuts" SEC03
  ; Start Menu Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  SetOutPath "$INSTDIR"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\LocheGSplicer.lnk"             "$INSTDIR\LocheGSplicer${CONFIG_NAME}.bat"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\README.lnk"                    "$INSTDIR\README.txt"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"                 "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
  
  ; Desktop Shortcuts
  CreateShortCut "$DESKTOP\LocheGSplicer.lnk"                             "$INSTDIR\LocheGSplicer${CONFIG_NAME}.bat"
SectionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                        Redistribution                       ;;;;
Section /o "Install VCRedist Package" VCRedistSection
  SetOutPath $INSTDIR
!ifdef VS2008
  File "vcredist_x86.exe"
  ExecWait "$INSTDIR\vcredist_x86.exe"
!endif
SectionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                         Shortcuts                           ;;;;
Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;                          Uninstall                          ;;;;
Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" $INSTDIR
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\ApplicationLoader.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"

  Delete "$INSTDIR\bin\*.*"
  RMDIR /r "$INSTDIR\bin"
  Delete "$INSTDIR\data\*.*"
  RMDIR /r "$INSTDIR\data"
  Delete "$INSTDIR\test\*.*"
  RMDIR /r "$INSTDIR\test"

  Delete "$INSTDIR\*.*"
  RMDIR /r "$INSTDIR"

  Delete "$SMPROGRAMS\$ICONS_GROUP\*.*"
  RMDIR /r "$SMPROGRAMS\$ICONS_GROUP"

  Delete "$DESKTOP\LocheGSplicer.lnk"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
