curl -s -b "_T_WM=86b5aa5f028887a1951a9c702b8661e7; WEIBOCN_WM=ig_1002; H5_INDEX=0_all" -A "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:39.0) Gecko/20100101 Firefox/39.0" $1 | pup "div.c [href^="http://weibo.cn/mblog/picAll"] attr{href}" \
| perl -i -pe "s/amp;//" \
| xargs -n1 curl -H "Host: weibo.cn" -H "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:39.0) Gecko/20100101 Firefox/39.0" -H "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" -H "Accept-Language: en-US,en;q=0.5" --compressed -H "Referer: http://weibo.cn/1651330523" -H "Cookie: _T_WM=1723bd66136652106e71cf96a2dba994; SUB=_2A254vmX3DeTxGedH7VEQ8SjNyT6IHXVYQQu_rDV6PUJbrdANLWjekW2FQ28-A6K-fXFK9Dv0IYSq2Ivpcg..; gsid_CTandWM=4ujs2a881NcvDGPTG25wb8eHa1Y" -H "Connection: keep-alive" | pup "[href^=/mblog/pic/] attr{href}" | sed -e 's/^/weibo.cn\//' \
| perl -i -pe "s/amp;//" \
| xargs -n1 curl -H "Host: weibo.cn" -H "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:39.0) Gecko/20100101 Firefox/39.0" -H "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" -H "Accept-Language: en-US,en;q=0.5" --compressed -H "Referer: http://weibo.cn/1651330523" -H "Cookie: _T_WM=1723bd66136652106e71cf96a2dba994; SUB=_2A254vmX3DeTxGedH7VEQ8SjNyT6IHXVYQQu_rDV6PUJbrdANLWjekW2FQ28-A6K-fXFK9Dv0IYSq2Ivpcg..; gsid_CTandWM=4ujs2a881NcvDGPTG25wb8eHa1Y" -H "Connection: keep-alive" | pup "div.c [href^="/mblog/oripic"] attr{href}" | sed -e 's/^/weibo.cn/' \
| perl -i -pe "s/amp;//" > templist.txt
cat templist.txt | sort -u > addlist.txt
./getwbloop
ls | sed -n 's/\(oripic?\)\(.*\)/mv "\1""\2" "\2".jpg/p' | sh

