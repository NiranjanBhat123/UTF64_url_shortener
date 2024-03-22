#include <iostream>
#include <string>
#include <mysql.h>
#include <cstdlib>
#include <sstream>
#include <algorithm>

using namespace std;

const string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

string encodeUrl(const string &originalUrl) {
    
    size_t startPos = originalUrl.find("://") + 3; 
    size_t endPos = originalUrl.find(".com");

    if (endPos == string::npos) {
        endPos = originalUrl.find("/", startPos);
    }

    string subUrl = originalUrl.substr(startPos, endPos - startPos);

   
    transform(subUrl.begin(), subUrl.end(), subUrl.begin(), ::tolower);

    
    stringstream ss;
    ss << base64_encode(reinterpret_cast<const unsigned char *>(subUrl.c_str()), subUrl.length());

    return ss.str();
}

inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}



int main() {
    MYSQL *conn;

    conn = mysql_init(NULL);

    if (!conn) {
        cerr << "MySQL initialization failed: " << mysql_error(conn) << endl;
        return 1;
    }

    conn = mysql_real_connect(conn, "localhost", "root", "8484", "urls", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return 1;
    }

    cout << "Connected to MySQL database successfully" << endl;

    // Create table
    if (mysql_query(conn, "CREATE TABLE IF NOT EXISTS url_shortener ("
                          "id INT PRIMARY KEY AUTO_INCREMENT,"
                          "original_url TEXT NOT NULL,"
                          "shortened_url TEXT NOT NULL)")) {
        cerr << "SQL error: " << mysql_error(conn) << endl;
        return 1;
    }

    string originalUrl;
    cout << "Enter the URL to shorten: ";
    cin >> originalUrl;

    string shortenedUrl = encodeUrl(originalUrl);

    string sqlInsert = "INSERT INTO url_shortener (original_url, shortened_url) VALUES ('" + originalUrl + "', '" + shortenedUrl + "');";

    if (mysql_query(conn, sqlInsert.c_str())) {
        cerr << "SQL error: " << mysql_error(conn) << endl;
        return 1;
    }

    cout << "URL shortened successfully: " << shortenedUrl << endl;

    
    string sqlSelect = "SELECT original_url FROM url_shortener WHERE shortened_url = '" + shortenedUrl + "';";
    if (mysql_query(conn, sqlSelect.c_str())) {
        cerr << "SQL error: " << mysql_error(conn) << endl;
        return 1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            cout << "Original URL for " << shortenedUrl << " is: " << row[0] << endl;
        }
        mysql_free_result(res);
    }

    mysql_close(conn);

    return 0;
}
