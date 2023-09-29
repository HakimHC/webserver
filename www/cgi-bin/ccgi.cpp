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
            } else {
                std::cout << "Content-Type: text/html\n\n";
                std::cout << "<html><body>\n";
                std::cout << "Animal not found.\n";
            }
        } else {
            std::cout << "Content-Type: text/html\n\n";
            std::cout << "<html><body>\n";
            std::cout << "Animal parameter not found in query string.\n";
        }
    } else {
        std::cout << "Content-Type: text/html\n\n";
        std::cout << "<html><body>\n";
        std::cout << "No query string provided.\n";
    }
	std::cout << "<form action=\"ccgi\" method=\"GET\">";
  	std::cout << "<label for=\"animal\">Enter an animal:</label>";
	std::cout << "<input type=\"text\" id=\"animal\" name=\"animal\">";
  	std::cout << "<button type=\"submit\">Submit</button>";
	std::cout << "</form>";
    std::cout << "</body></html>\n";
    return 0;
}