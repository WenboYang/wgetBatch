call rm yz
call wget weibo.cn/2214838982 -O ksn.gz
call gzip -d ksn.gz 
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
For /f "tokens=1-4 delims=/:" %%a in ('time /t') do (set mytime=%%a%%b%%c%%d)
call mv ksn ksn%mydate%_%mytime%_%RANDOM%.html
