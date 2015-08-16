curl "www.dianping.com/shop/$1" | pup ul."comment-list" json{} > "samples/$1.json"
