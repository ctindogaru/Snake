# Snake

Pentru a putea rula jocul veti avea nevoie de libraria "curses" care poate fi instalata tastand in linia de comanda: "sudo apt-get install libncurses5-dev libncursesw5-dev".
Pentru a juca scrieti in terminal: make -> make run -> alegeti modul de joc -> have fun and watch your tail :).

Codul are 2 parti principale: snake cu obstacole si snake fara obstacole. (coduri asemanatoare la ambele + cateva conditii in plus pentru cel cu obstacole)
Jocul va incepe cu sarpele miscandu-se automat spre dreapta, daca va lua o bucata de hrana, acesta se va mari cu o unitate, iar la fiecare 2 bucati de hrana obtinute viteza ii va creste. Daca sarpele va face o misca invalida cum ar fi sa se atinga pe el insusi, sa atinga un obstacol sau zidul jocul va lua sfarsit. Am considerat ca daca sarpele va merge spre directia d si va fi apasata tasta a, jocul va lua sfarsit deoarece sarpele se musca singur.
Generarea de hrana nu se va face intamplator ci se va verifica ca aceasta sa nu fie generata pe un obstacol/zid/sarpe.
