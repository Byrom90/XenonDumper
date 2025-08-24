@if exist XD_res.xzp del XD_res.xzp

@pushd COMPLETED_RESOURCES

@set XUIPKG="%XEDK%\bin\win32\xuipkg.exe" /nologo

@set XUI_XZP="%~dp0%XD_res.xzp"
@set XUI_FILES=*.*

@echo Building XD_res.xzp

%XUIPKG% /R /O %XUI_XZP% %XUI_FILES%

@popd

@cmd /k