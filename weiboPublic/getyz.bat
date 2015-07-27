call rm yz
call wget weibo.cn/1841158660 -O yz.gz
call gzip -d yz.gz 
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
For /f "tokens=1-4 delims=/:" %%a in ('time /t') do (set mytime=%%a%%b%%c%%d)
call ren yz yz%mydate%_%mytime%_%RANDOM%.html
