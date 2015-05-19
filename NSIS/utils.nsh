;------------------------------------------------------------------------------
; Utils
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Run application with dropped privileges.
;!include WinVer.nsh
Function RunExecutable
  Pop $0
  Pop $1
  ShellExecAsUser::ShellExecAsUser "open" "$0" 'infinit://fingerprint/$1'
FunctionEnd

;------------------------------------------------------------------------------
; Parse the command line.
;!include FileFunc.nsh
;!insertmacro GetParameters
;!insertmacro GetOptions
Function ParseCommandline
  ${GetParameters} $R0
  ${If} ${Errors}
    MessageBox MB_OK "An error occured"
  ${EndIf}
  ${GetOptions} $R0 "/autoupdate" $R1
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
Function AlreadyInstalled
  ReadRegStr $PreviousInstallDir HKLM "${regkey}" "InstallDir"

  ${If} $PreviousInstallDir != ""

    ; Make sure directory is valid
    Push $R0
    Push $R1
    StrCpy $R0 "$PreviousInstallDir" "" -1
    ${If} $R0 == '\'
    ${OrIf} $R0 == '/'
      StrCpy $R0 $PreviousInstallDir*.*
    ${Else}
      StrCpy $R0 $PreviousInstallDir\*.*
    ${EndIf}
    ${IfNot} ${FileExists} $R0
      StrCpy $PreviousInstallDir ""
    ${EndIf}
    Pop $R1
    Pop $R0

  ${EndIf}
FunctionEnd

;------------------------------------------------------------------------------
; Read the customer data at the end of the installer.
Function ReadCustomerData
  ; arguments
  Exch $R1            ; customer data magic value
  ; locals
  Push $1             ; file name or (later) file handle
  Push $2             ; current trial offset
  Push $3             ; current trial string (which will match $R1 when customer data is found)
  Push $4             ; length of $R1

  FileOpen $1 $EXEPATH r

; change 1024 here to, e.g., 2048 to scan the last 2Kb of EXE file
  IntOp $2 0 - 1024
  StrLen $4 $R1

loop:
  FileSeek $1 $2 END
  FileRead $1 $3 $4
  StrCmp $3 $R1 found
  IntOp $2 $2 + 1
  IntCmp $2 0 loop loop

  StrCpy $R1 ""
  goto fin

found:
  IntOp $2 $2 + $4
  FileSeek $1 $2 END
  FileRead $1 $3
  StrCpy $R1 $3

fin:
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $R1
FunctionEnd
