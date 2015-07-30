#get product name and price
curl -s $1 | pup "div.head", "div.sale b" text{}

#download picture"
curl -s -O `curl -s $1 | pup "div.big_pic img attr{src}"`


