@echo off
for %%f in (*.jks) do (
    openssl base64 < %%f | tr -d '\n' | tee %%~nf.b64
)