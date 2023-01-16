Aplicatie server-client de tip multithreading.
In cadrul clientului pot fi date comenzile:
    a)  LIST/list: afiseaza fisierele din directorul serverului impreuna cu     dimensiunea fisierului;
    b)  GET/get <filename>: muta un fisier din directorul serverului in directorul clientului;
    c)  PUT/put <filename>: muta un fisier din directorul clientului in directorul serverului;
    d)  DELETE/delete <filename>: sterge un director de pe directorul serverului;
    e)  UPDATE/update <filename>: actualizeaza continut dintr-un fisier de pe directorul serverului de la un octet introdus de client;
    f)  SEARCH/search <word>: cauta aparitia unui cuvant in toate fisierele stocate in memorie de catre thread-ul ce se ocupa cu acest lucru;
    g)  EXIT/exit: comanda pe care o da clientul pentru a se deconecta--serverul ramane treaz;

