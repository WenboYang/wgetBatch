#get product id
curl -s $1 | pup "div.main div.head h1, div.sale span#salePriceTag text{}"

#download picture"
curl -s -O `curl -s $1 | pup "div.main div.head h1, div.sale spen#salePriceTag, div.big_pic img attr{src}"`


