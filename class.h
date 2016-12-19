#include <vector.h>
#include <unordered_map>

typedef std::unordered_map <std::string, std::string> HashList;
//typedef std::vector <hash_item> hash;
//
//class Args {
//    hash args;
//    
//    Args() {
//        args.empty();
//    };
//};

std::string Serialize(HashList &h) {
    std::string ret;
    unsigned char *tmp = (unsigned char *) malloc(1024)(0);
    for (auto i = h.begin(); i != h.end(); i++)
    {
        std::string tmp;
        
        (*i).first
        (*i).second
    }
}

class InterfaceInfo {
  HashList args;
   
  InterfaceInfo() {
    args["ifname"] = "undef";
    args["iftype"] = "undef";    
    args["ipaddr"] = "undef";    
    args["hwaddr"] = "undef";
    args["mask"] = "undef";
  };
  
  
};