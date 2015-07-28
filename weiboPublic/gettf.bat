rm yz
wget weibo.cn/1400229064 -O yz.gz
gzip -d yz.gz 
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
For /f "tokens=1-4 delims=/:" %%a in ('time /t') do (set mytime=%%a%%b%%c%%d)
mv yz tf%mydate%_%mytime%_%RANDOM%.html
