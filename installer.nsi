
!define company "Infinit.io"
!define website "infinit.io"

!define prodname "Infinit"

!define build_type debug
!define build_dir "build\${build_type}"
!define src_dir "."
!define bin "Infinit.exe"
!define libgap "libgap.dll"
!define resources_dir "resources"

; External resources
!define qt_build_type ${build_type}
!if ${qt_build_type} == debug
	!define qt_suffix "d"
!else
	!define qt_suffix ""
!endif

!define qt_dir "c:\Users\hotgloupi\Downloads\qt-everywhere-opensource-src-4.8.5\lib"
!define qt_core "QtCore${qt_suffix}4.dll"
!define qt_gui "QtGui${qt_suffix}4.dll"
!define qt_network "QtNetwork${qt_suffix}4.dll"

; license text file
;!define licensefile "${src_dir}\LICENSE"

; icons must be Microsoft .ICO files
;!define icon "icon.ico"

; installer background screen
;!define screenimage "${resources_dir}\install-background.bmp"

; Release notes
;!define notefile "${src_dir}\Release notes.txt"

; Documentation
;!define helpfile "${src_dir}\Documentation.doc"

; Registry
!define regkey "Software\${company}\${prodname}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"

!define startmenu "$SMPROGRAMS\${company}\${prodname}"

!define setup "infinit-install.exe"
!define uninstaller "uninstall.exe"


;------------------------------------------------------------------------------
; General configuration
;------------------------------------------------------------------------------

XPStyle on
ShowInstDetails show
ShowUninstDetails hide
AutoCloseWindow false

Name "${prodname}"
Caption "${prodname}"

!ifdef icon
	Icon "${icon}"
!endif

OutFile "${setup}"

SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal

InstallDir "$PROGRAMFILES\${company}\${prodname}"
InstallDirRegKey HKLM "${regkey}" ""

!ifdef licensefile
	LicenseText "License"
	LicenseData "${licensefile}"
!endif

; Set the text which prompts the user to enter the installation directory
DirText "Choose the install directory"

UninstallText "This will uninstall ${prodname}."
!ifdef icon
	UninstallIcon "${icon}"
!endif

;------------------------------------------------------------------------------
; Pages
;------------------------------------------------------------------------------

!ifdef licensefile
	Page license
!endif

; Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;------------------------------------------------------------------------------
; Installer background
;------------------------------------------------------------------------------

!ifdef screenimage

	Function .onGUIInit
		; extract background BMP into temp plugin directory
		InitPluginsDir
		File /oname=$PLUGINSDIR\1.bmp "${screenimage}"

		BgImage::SetBg /NOUNLOAD /FILLSCREEN $PLUGINSDIR\1.bmp
		BgImage::Redraw /NOUNLOAD
	FunctionEnd

	Function .onGUIEnd
		; Destroy must not have /NOUNLOAD so NSIS will be able to unload and
		; delete BgImage before it exits
		BgImage::Destroy
	FunctionEnd

!endif

;------------------------------------------------------------------------------
; Install section
;------------------------------------------------------------------------------

Section "Install Infinit"

	WriteRegStr HKLM "${regkey}" "Install_Dir" "$INSTDIR"
	WriteRegStr HKLM "${uninstkey}" "DisplayName" "${prodname} (remove only)"
	WriteRegStr HKLM "${uninstkey}" "UninstallString" '"$INSTDIR\${uninstaller}"'

	!ifdef filetype
		WriteRegStr HKCR "${filetype}" "" "${prodname}"
	!endif

	WriteRegStr HKCR "${prodname}\Shell\open\command\" "" '"$INSTDIR\${bin} "%1"'

	!ifdef icon
		WriteRegStr HKCR "${prodname}\DefaultIcon" "" "$INSTDIR\${icon}"
	!endif

	; Main binaries
	SetOutPath $INSTDIR
	File /a "${build_dir}\${bin}"
	File /a "${build_dir}\${libgap}"
	File /a "${qt_dir}\${qt_core}"
	File /a "${qt_dir}\${qt_gui}"
	File /a "${qt_dir}\${qt_network}"

	; resources
	SetOutPath "$INSTDIR\${resources_dir}"
	File /a /r "${src_dir}\${resources_dir}\"

	; Optional files
	SetOutPath $INSTDIR
	!ifdef licensefile
		File /a "${src_dir}\${licensefile}"
	!endif

	!ifdef notefile
		File /a "${src_dir}\${notefile}"
	!endif

	!ifdef icon
		File /a "${resources_dir}\${icon}"
	!endif

	WriteUninstaller "${uninstaller}"

SectionEnd


;------------------------------------------------------------------------------
; Create shortcuts section
;------------------------------------------------------------------------------

Section
	CreateDirectory "${startmenu}"
	SetOutPath $INSTDIR ; for working directory
	!ifdef icon
		CreateShortCut "${startmenu}\${prodname}.lnk" "$INSTDIR\${bin}" "" "$INSTDIR\${icon}"
	!else
		CreateShortCut "${startmenu}\${prodname}.lnk" "$INSTDIR\${bin}"
	!endif

	!ifdef notefile
		CreateShortCut "${startmenu}\Release Notes.lnk" "$INSTDIR\${notefile}"
	!endif

	!ifdef helpfile
		CreateShortCut "${startmenu}\Documentation.lnk" "$INSTDIR\${helpfile}"
	!endif

	!ifdef website
		WriteINIStr "${startmenu}\web site.url" "InternetShortcut" "URL" ${website}
		; CreateShortCut "${startmenu}\Web Site.lnk "${website}" "URL"
	!endif

	!ifdef notefile
		ExecShell "open" "$INSTDIR\${notefile}"
	!endif
SectionEnd

;------------------------------------------------------------------------------
; Uninstall section
;------------------------------------------------------------------------------

Section "Uninstall"
	DeleteRegKey HKLM "${uninstkey}"
	DeleteRegKey HKLM "${regkey}"

	Delete "${startmenu}\*.*"
	Delete "${startmenu}"

	!ifdef licensefile
		Delete "$INSTDIR\${licensefile}"
	!endif

	!ifdef notefile
		Delete "$INSTDIR\${notefile}"
	!endif

	!ifdef icon
		Delete "$INSTDIR\${icon}"
	!endif

	Delete "$INSTDIR\${bin}"
	Delete "$INSTDIR\${libgap}"

	RMDir "$INSTDIR"
SectionEnd



