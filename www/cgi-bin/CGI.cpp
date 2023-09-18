#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    char* queryString = std::getenv("QUERY_STRING");

    if (queryString) {
        std::string query(queryString);
        std::string animalParam = "animal=";
        std::size_t found = query.find(animalParam);

        if (found != std::string::npos) {
            std::string animal = query.substr(found + animalParam.size());

            if (animal == "cat" || animal == "dog") {
                std::cout << "Content-Type: text/html\n\n";
                std::cout << "<html><body>\n";
                std::cout << "<img src=\"" << animal << ".jpg\" alt=\"" << animal << " image\">\n";
                std::cout << "</body></html>\n";
            } else {
                std::cout << "Content-Type: text/html\n\n";
                std::cout << "<html><body>\n";
                std::cout << "Animal not found.\n";
                std::cout << "</body></html>\n";
            }
        } else {
            std::cout << "Content-Type: text/html\n\n";
            std::cout << "<html><body>\n";
            std::cout << "Animal parameter not found in query string.\n";
            std::cout << "</body></html>\n";
        }
    } else {
        std::cout << "Content-Type: text/html\n\n";
        std::cout << "<html><body>\n";
        std::cout << "No query string provided.\n";
        std::cout << "</body></html>\n";
    }

    return 0;
}