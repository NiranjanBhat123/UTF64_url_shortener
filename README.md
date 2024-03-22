# URL shortner

A simple URL shortener written in C++ with MySQL database and base64 encoding.

Windows operating system Administrative rights to install software C++ compiler (e.g., MinGW, Visual Studio) MySQL server

# Installation

MySQL Installation
* Download MySQL installer from the official MySQL website.
* Run the installer and follow the on-screen instructions to complete the installation.
* During installation, set a root password for MySQL.
* MySQL Connector for C++ Installation
* Download MySQL Connector/C++ from the official MySQL Connector/C++ website.
* Run the installer and follow the on-screen instructions to complete the installation.
commands to run the code

* g++ -std=c++11 main.cpp -o shorten_url -IC:\path\to\mysql\include -LC:\path\to\mysql\lib -lmysqlclient
* ./shorten_url
