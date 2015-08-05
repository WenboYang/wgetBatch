// Hello World example
// This example shows basic usage of DOM-style API.

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <cstdio>
#include "rapidjson/reader.h"
#include <iostream>
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;
using namespace std;

int main(int, char*[]) {

    // Prepare reader and input stream.
    Reader reader;
    char readBuffer[65536];
    FileReadStream is(stdin, readBuffer, sizeof(readBuffer));

    ////////////////////////////////////////////////////////////////////////////
    // 1. Parse a JSON text string to a document.

    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    printf("Original JSON:\n %s\n", json);

    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

#if 1
    // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
    if (document.ParseStream(is).HasParseError())
        return 1;
#else
    // In-situ parsing, decode strings directly in the source string. Source must be string.
    char buffer[sizeof(json)];
    memcpy(buffer, json, sizeof(json));
    if (document.ParseInsitu(buffer).HasParseError())
        return 1;
#endif

    printf("\nParsing to document succeeded.\n");

    ////////////////////////////////////////////////////////////////////////////
    // 2. Access values in document. 

    printf("\nAccess values in document:\n");
    assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.
    {
        const Value& a = document["users"]; // Using a reference for consecutive access is handy and faster.
        assert(a.IsArray());
        for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
            printf("user id&name =%s %s", a[i]["idstr"].GetString(), a[i]["name"].GetString()  ); 
#if 0
        
        int y = a[0].GetInt();
        (void)y;

        // Iterating array with iterators
        printf("a = ");
        for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
            printf("%d ", itr->GetInt());
        printf("\n");
#endif
    }
#if 0

    // Iterating object members
    static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
    for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
        printf("Type of member %s is %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);

    ////////////////////////////////////////////////////////////////////////////
    // 3. Modify values in document.

    // Change i to a bigger number
    {
        uint64_t f20 = 1;   // compute factorial of 20
        for (uint64_t j = 1; j <= 20; j++)
            f20 *= j;
        document["i"] = f20;    // Alternate form: document["i"].SetUint64(f20)
        assert(!document["i"].IsInt()); // No longer can be cast as int or uint.
    }

    // Adding values to array.
    {
        Value& a = document["a"];   // This time we uses non-const reference.
        Document::AllocatorType& allocator = document.GetAllocator();
        for (int i = 5; i <= 10; i++)
            a.PushBack(i, allocator);   // May look a bit strange, allocator is needed for potentially realloc. We normally uses the document's.

        // Fluent API
        a.PushBack("Lua", allocator).PushBack("Mio", allocator);
    }

    // Making string values.

    // This version of SetString() just store the pointer to the string.
    // So it is for literal and string that exists within value's life-cycle.
    {
        document["hello"] = "rapidjson";    // This will invoke strlen()
        // Faster version:
        // document["hello"].SetString("rapidjson", 9);
    }

    // This version of SetString() needs an allocator, which means it will allocate a new buffer and copy the the string into the buffer.
    Value author;
    {
        char buffer[10];
        int len = sprintf(buffer, "%s %s", "Milo", "Yip");  // synthetic example of dynamically created string.

        author.SetString(buffer, static_cast<size_t>(len), document.GetAllocator());
        // Shorter but slower version:
        // document["hello"].SetString(buffer, document.GetAllocator());

        // Constructor version: 
        // Value author(buffer, len, document.GetAllocator());
        // Value author(buffer, document.GetAllocator());
        memset(buffer, 0, sizeof(buffer)); // For demonstration purpose.
    }
    // Variable 'buffer' is unusable now but 'author' has already made a copy.
    document.AddMember("author", author, document.GetAllocator());

    assert(author.IsNull());        // Move semantic for assignment. After this variable is assigned as a member, the variable becomes null.

    ////////////////////////////////////////////////////////////////////////////
    // 4. Stringify JSON

    printf("\nModified JSON with reformatting:\n");
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    puts(sb.GetString());

#endif
    return 0;
}
