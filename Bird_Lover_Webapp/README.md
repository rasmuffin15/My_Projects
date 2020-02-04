# WingZ.Code
The Code. For Wingz.
WingZ is a program to report and track birds in your area.
Repo Organization:
WingZ_EJS has all the views as well as the servers. The css files are within resources. The script is also within 
resources under the js file.
How to build/run:

Download and install PostgreSQL:

In the flavor required for your OS, download and
install the most current stable version of PostgreSQL. It's important to note for this program that postgreSQL is referenced on Port 5433 default for Windows, Port 5432 is the default for mac so if it isn't letting you signin the index.js file contains the port to change. Your installation may require modification for this to work properly.

Next, enter the psql terminal, and in the default DB (postgres), copy and paste the SQL from WingzStructure.sql into the terminal, creating the tables and structures required.
Follow this with wingzData, prepopulating the database with appropriate data.

Finally, to start the service, run the www.js file AS JAVASCRIPT (node www) from the bin folder. The service will secure port 3000, and is ready to go!