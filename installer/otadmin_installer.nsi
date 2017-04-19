#--------------------------------
# Include Modern UI
	!include "MUI2.nsh"
#--------------------------------

# GENERAL
	!define OTADMIN_VERSION "0.0.1"
	!define OTADMIN_NAME "OTAdmin++" 

	; Info
	Name "${OTADMIN_NAME} ${OTADMIN_VERSION}"
	OutFile "otadmin-v${OTADMIN_VERSION}.exe"
	Icon "otadmin.ico"
	!define uninstaller "uninstall.exe"

	; Icons
	!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
	!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-full.ico"

	; Product info
	!define MUI_PRODUCT "OTAdmin++"
	!define MUI_FILE "OTAdmin++"
	!define MUI_VERSION "0.0.1"
	!define MUI_BRANDINGTEXT "OTAdmin++ v0.0.1"


	; Default installation folder
	InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"

	; Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\${MUI_PRODUCT}" ""
	!define regkey "Software\${MUI_PRODUCT}"
	!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"

	; Request application privileges for Windows Vista
	RequestExecutionLevel user

#--------------------------------
# VERSION informations
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${OTADMIN_NAME}"
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Open tibia remote control tool."
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "OTSoft.org"
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "Test Application is a trademark of Fake company"
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© Fake company"
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${OTADMIN_NAME}"
	#VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${OTADMIN_VERSION}"

#--------------------------------
# Interface Settings

	!define MUI_ABORTWARNING

	; Auto run otadmin
	!define MUI_FINISHPAGE_RUN "$INSTDIR\OTAdmin++.exe"

	; link to website
	!define MUI_FINISHPAGE_LINK "Visit OTAdmin++ website"
	!define MUI_FINISHPAGE_LINK_LOCATION "http://otadmin.otsoft.org"
#--------------------------------
# PAGES

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH

#--------------------------------
# Languages

	!insertmacro MUI_LANGUAGE "English"

#--------------------------------
# Installer Sections

Section "${OTADMIN_NAME}" SectionMain

	SetOutPath "$INSTDIR"

	; Files
	File /oname=OTAdmin++.exe OTAdmin.exe
	File otadmin.ico
	File LICENSE.txt
	File CHANGELOG.txt

	; Create uninstaller
	WriteUninstaller "$INSTDIR\${uninstaller}"

	; Create startmenu shortcut
	CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe"
	; Create desktop shortcut
	CreateShortCut "$DESKTOP\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" ""

	; Store installation folder
	WriteRegStr HKCU "Software\${OTADMIN_NAME}" "" $INSTDIR
	WriteRegStr HKLM "${regkey}" "Install_Dir" "$INSTDIR"

	; write uninstall strings
	WriteRegStr HKLM "${uninstkey}" "DisplayName" "${MUI_PRODUCT}"
	WriteRegStr HKLM "${uninstkey}" "UninstallString" '"$INSTDIR\${uninstaller}"'
SectionEnd

Section "Libraries" SectionLibraries
	SetOutPath "$INSTDIR"
	File libs\*
SectionEnd

;Section "Sources" SectionSources
;	SetOutPath "$INSTDIR\sources"
;	File sources\*.cpp
;	File sources\*.h

;	SetOutPath "$INSTDIR\sources\images"
;	File sources\images\*.png
;SectionEnd

Section "Examples" SectionExamples
	SetOutPath "$INSTDIR\scripts"
	File scripts\*
SectionEnd

#--------------------------------
# Descriptions

	;Language strings
	LangString DESC_SectionMain ${LANG_ENGLISH} "Program main files."
	LangString DESC_SectionLibraries ${LANG_ENGLISH} "DLL Libraries required by the program."
	LangString DESC_SectionSources ${LANG_ENGLISH} "Source code."
	LangString DESC_SectionExamples ${LANG_ENGLISH} "Example scripts."
	;LangString DESC_SectionShortcutStartMenu ${LANG_ENGLISH} "Create start menu shortcut."
	;LangString DESC_SectionShortcutDesktop ${LANG_ENGLISH} "Create desktop shortcut."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${SectionMain} $(DESC_SectionMain)
		!insertmacro MUI_DESCRIPTION_TEXT ${SectionLibraries} $(DESC_SectionLibraries)
		;!insertmacro MUI_DESCRIPTION_TEXT ${SectionSources} $(DESC_SectionSources)
		!insertmacro MUI_DESCRIPTION_TEXT ${SectionExamples} $(DESC_SectionExamples)
		;!insertmacro MUI_DESCRIPTION_TEXT ${SectionShortcutStartMenu} $(DESC_SectionShortcutStartMenu)
		;!insertmacro MUI_DESCRIPTION_TEXT ${SectionShortcutDesktop} $(DESC_SectionShortcutDesktop)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
#Uninstaller Section

Section "Uninstall"

	; delete uninstall file
	Delete "$INSTDIR\${uninstaller}"

	; remove the link from the start menu
	delete "$SMPROGRAMS\${MUI_PRODUCT}.lnk"
	; and desktop..
	delete "$DESKTOP\${MUI_PRODUCT}.lnk"

	; clean program files directory
	RMDir /r "$INSTDIR"

	; clean registry
	DeleteRegKey HKLM "${uninstkey}"
	DeleteRegKey HKLM "${regkey}"
	DeleteRegKey /ifempty HKCU "Software\${OTADMIN_NAME}"

SectionEnd
