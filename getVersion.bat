@echo off
FOR /F "tokens=1 delims=" %%A in ('git describe --tag --always --dirty') do echo -D VERSION=\"%1-%%A\"
