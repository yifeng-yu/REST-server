#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <ostream>
#include <fstream>
using namespace std;

#define TRACE(msg)            wcout << msg

class MovieDB{
public:
    std::string ID;
    std::string Title;
    std::string Year;
    std::string Runtime;
    std::string URL;
    
};
map<std::string, MovieDB*> dbIDMap;
map<std::string, vector<MovieDB*>> dbNameMap;

void parseFile(std::string filename){
    
    ifstream file(filename);
    std::string line;
    stringstream ss;
    while(true){
        MovieDB* db = new MovieDB;
        std::vector<std::string> tempMovie;
        std::getline(file,line);
        if(file.fail()){
            break;
        }
        const char delim = '\t';
        size_t start = 0;
        size_t delimLoc = line.find_first_of(delim,start);
        while(delimLoc != std::string::npos){
            tempMovie.emplace_back(line.substr(start,delimLoc - start));
            start = delimLoc+1;
            delimLoc = line.find_first_of(delim,start);
        }
        tempMovie.emplace_back(line.substr(start));
        if(tempMovie[1] != "movie") continue;
//        for(size_t i = 0; i < tempMovie.size(); i++){
//            std::cout << tempMovie[i] << std::endl;
//        }
        std::string id = tempMovie[0];
        std::string title = tempMovie[2];
        db-> ID = id;
        db-> Runtime = tempMovie[7];
        db-> Year = tempMovie[5];
        db-> Title = tempMovie[2];
        db->URL = "https://www.imdb.com/title/" + id + "/";
        
//                std::cout << db->URL <<std::endl;
//                std::cout << db->Runtime <<std::endl;
//                std::cout << db-> Year <<std::endl;
//                std::cout << db-> Title <<std::endl;
//
        dbIDMap.insert(std::make_pair(id, db));
        
        // for name
        std::vector<MovieDB*> mov;
        if(dbNameMap.find(title) == dbNameMap.end()){
            mov.emplace_back(db);
            dbNameMap.insert(std::make_pair(title, mov));
        }
        else {
            dbNameMap.find(title)->second.emplace_back(db);
        }

    }
    
}
map<utility::string_t, utility::string_t> dictionary;


vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void handle_get(http_request request)
{
   TRACE(L"\nhandle GET\n");
    
   
    std::string line = request.request_uri().to_string();
    std::cout << request.request_uri().to_string() << std::endl;
    std::vector<std::string> uriVec;
    
    const char delim = '/';
    size_t start = 0;
    size_t delimLoc = line.find_first_of(delim,start);
    while(delimLoc != std::string::npos){
        uriVec.emplace_back(line.substr(start,delimLoc - start));
        start = delimLoc+1;
        delimLoc = line.find_first_of(delim,start);
    }
    uriVec.emplace_back(line.substr(start));
    std::string dis = uriVec[uriVec.size()-2];
    std::string id = uriVec[uriVec.size()-1];
    std::cout << id << " " << dis <<std::endl;
    
    if(dis == "name"){
        auto answer = json::value::array();
        std::string nameLookUp = id;
        std::string realName;
        if(id.find("%20") != std::string::npos){
            realName += split(nameLookUp, "%20")[0];
            realName += " ";
            realName += split(nameLookUp, "%20")[1];
            std::cout << realName << std::endl;
            
        }
        else {
            realName = id;
        }
        // find it
        if (dbNameMap.find(realName) != dbNameMap.end()){
            std::vector<MovieDB*> vec = dbNameMap.find(realName)->second;
            auto eachAns = json::value::object();
            for(size_t i = 0; i < vec.size(); i++){
                MovieDB* db = vec[i];
                eachAns["ID"] = json::value::string(db->ID);
                if(db->Runtime.find("\\N") != std::string::npos){
                    eachAns["Runtime"] = json::value(0);
                }
                else eachAns["Runtime"] = json::value(stoi(db->Runtime));
                eachAns["Title"] = json::value::string(db->Title);
                eachAns["URL"] = json::value::string(db->URL);
                if(db->Year.find("\\N") != std::string::npos){
                    eachAns["Year"] = json::value(0);
                }
                else eachAns["Year"] = json::value(stoi(db->Year));
                //request.reply(status_codes::OK, eachAns);
                answer[i] = eachAns;
                
            }

            request.reply(status_codes::OK, answer);
        }
        else {
            auto answer = json::value::object();
            answer["Error"] = json::value::string("No movie by that name found");
            request.reply(404, answer);
        }
        
    }
    else {
        auto answer = json::value::object();
        if (dbIDMap.find(id) != dbIDMap.end()){
            MovieDB* db = dbIDMap.find(id)->second;
            answer["ID"] = json::value::string(db->ID);
            if(db->Runtime.find("\\N") != std::string::npos){
                answer["Runtime"] = json::value(0);
            }
            else answer["Runtime"] = json::value(stoi(db->Runtime));
            answer["Title"] = json::value::string(db->Title);
            answer["URL"] = json::value::string(db->URL);
            if(db->Year.find("\\N") != std::string::npos){
                answer["Year"] = json::value(0);
            }
            else answer["Year"] = json::value(stoi(db->Year));
            request.reply(status_codes::OK, answer);
            
        }
        else {
            answer["Error"] = json::value::string("No movie with that ID found");
            request.reply(404, answer);
        }
        
    }
    
}


/* handlers implementation */

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Implement
    std::string filename = argv[1];
    parseFile(filename);
    http_listener listener("http://localhost:12345/");

    listener.support(methods::GET, handle_get);

    try
    {
       listener
          .open()
          .then([&listener](){TRACE(L"\nstarting to listen\n");})
          .wait();

       while (true);
    }
    catch (exception const & e)
    {
       wcout << e.what() << endl;
    }

}
