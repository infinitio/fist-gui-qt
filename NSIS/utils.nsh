;------------------------------------------------------------------------------
; Utils
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Kill running application if any.
;!include nsProcess.nsh
Function KillRunningProcess
    Pop $0
    ${nsProcess::FindProcess} $0 $R0
    StrCmp $R0 0 0 end
    ${nsProcess::CloseProcess} $0 $R0
    StrCmp $R0 0 end 0
    MessageBox MB_OK "Unable to kill $0"
    end:
      ${nsProcess::Unload}
FunctionEnd

;------------------------------------------------------------------------------
; Run application with dropped privileges.
;!include WinVer.nsh
Function RunExecutable
  Pop $0
  ${If} ${AtLeastWinVista}
    Exec '"$WINDIR\explorer.exe" $0'
  ${Else}
    Exec '$0'
  ${Endif}
FunctionEnd

;------------------------------------------------------------------------------
; Parse the command line to determine if update is present.
;!include FileFunc.nsh
;!insertmacro GetParameters
;!insertmacro GetOptions
Function ParseCommandline
  ${GetParameters} $R0
  ${GetOptions} $R0 "/update" $AutoUpdate
  ${If} ${Errors}
    StrCpy $AutoUpdate 0
  ${Else}
    StrCpy $AutoUpdate 1
  ${EndIf}
FunctionEnd

;------------------------------------------------------------------------------
; Check if the user is running installer on 32 of 64 windows.
;!include x64.nsh
Function CheckArchitecture
  ; XXX: Template to pop an error is the installer bundles the 32-bit and the
  ; plateform is 64-bit and vice versa.
  ; As long as we only use the 32-bit version, this is not mandatory.
  ${If} ${RunningX64}
  ${Else}
  ${Endif}
FunctionEnd

;------------------------------------------------------------------------------
; Check if a version has already been installed and is still valid.
; Function AlreadyInstalled
;   ReadRegStr $PreviousInstallDir HKLM "${regkey}" "InstallDir"
;
;   ${If} $PreviousInstallDir != ""
;
;     ; Make sure directory is valid
;     Push $R0
;     Push $R1
;     StrCpy $R0 "$PreviousInstallDir" "" -1
;     ${If} $R0 == '\'  ;'
;     ${OrIf} $R0 == '/'
;       StrCpy $R0 $PreviousInstallDir*.*
;     ${Else}
;       StrCpy $R0 $PreviousInstallDir\*.*
;     ${EndIf}
;     ${IfNot} ${FileExists} $R0
;       StrCpy $PreviousInstallDir ""
;     ${EndIf}
;     Pop $R1
;     Pop $R0
;
;   ${EndIf}
; FunctionEnd
