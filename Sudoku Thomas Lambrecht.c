#include <stdio.h>

const INFO = 0;                           // bij 1 gaat alles in stappen, bij 0 draait alles op de achtergrond
char sudoku[] = "Sudoku.txt";
char sudoku2[] = "Solved_Sudoku.txt";

FILE *fp;                               // fp = file pointer
void display(int S[][10], int dots);
void empty(int S[][10]);
int fill(int S[][10]);
int check(int S[][10]);
int guess(int S[][10]);
void Pause();

int count = 0;

int main()
{
    // -- SETUP --
    int i,j,number;
    char line[82];
    int S[81][10];
    
    // -- INLEZEN -- 
    
    // Sudoku lezen uit bestand sudoku.txt (formaat = volledige sudoku lezen, nummer voor nummer en een 0 voor niets)
    // Deze sudoku:
    // 4  .  . | .  .  . | 8  .  5 
    // .  3  . | .  .  . | .  .  . 
    // .  .  . | 7  .  . | .  .  . 
    //---------+---------+---------
    // .  2  . | .  .  . | .  6  . 
    // .  .  . | .  8  . | 4  .  . 
    // .  .  . | .  1  . | .  .  . 
    //---------+---------+---------
    // .  .  . | 6  .  3 | .  7  . 
    // 5  .  . | 2  .  . | .  .  . 
    // 1  .  4 | .  .  . | .  .  . 
    // wordt: '400000805030000000000700000020000060000080400000010000000603070500200000104000000'
    fp = fopen (sudoku, "rt");              // Open file, rt = read text
    printf ("\nIngelezen sudoku: %s:",sudoku);
    for (i = 0; i < 81; ++i)
    {
        fscanf(fp, "%1d", &number);
        printf(number);
        for (j = 0; j < 10; ++j)
        {
            if (number != 0)
                if (j == 0 || j == number)
                    S[i][j] = number;       // S[i] = number op plaats 0 en op plaats number (bvb {3,0,0,3,0,0,0,0,0,0})
                else
                    S[i][j] = 0;
            else
                S[i][j] = j;                // S[i] = {0,1,2,3,4,5,6,7,8,9} voor alle onzekere waarden van i
        }                                   //      = alle mogelijkheiden die op locatie i in de sudoku kunnen staan
    }
    printf ("\n\n");
    fclose(fp);                         // Sluit de file
    display(S, 1);                      // print de array S af, 1 voor alleen de zeker waarden (op de andere plaatsen een '.'), 0 om alle mogelijke waarden te tonen
    if (INFO == 1)
            display(S, 0);
    
    // -- LOGISCH OPLOSSEN -- 
    
    printf ("Oplossing zoeken via logische regels:\n");
    int finished = 0;
    while(finished == 0)
    {
        empty(S);                       // Haalt de gekende waarden uit de mogelijkheden van locaties in dezelfde rij, kolom of vierkant
        if (INFO == 1)
        {
            display(S, 0);
            Pause();
        }
        finished = fill(S);             // Checkt op elke locatie of er genoeg mogelijkheden geschrapt zijn om met zekerheid te zeggen welke waarde er moet komen
                                        //  Returned een 0 als er geen nieuwe waarden meer gevonden worden, indien wel kunnen deze opnieuw geschrapt worden (daarom de while lus)
        if (INFO == 1)
        {
            display(S, 1);
            Pause();
        }
    }                                  
    display(S, 1);
    if (INFO == 1)
        display(S, 0);
    
    // -- TRIAL AND ERROR --
    
    if (check(S) == 0)                  // Checkt of de array opgelost is, 0 voor niet opgelost, 1 voor opgelost en 2 voor een fout in de sudoku
    {
        printf ("Nog niet klaar.\nOplossing zoeken via gokken:\n");
        guess(S);                       // Loopt alle mogelijkheden af voor de niet ingevulde plaatsen, bij elke ingevulde waarde worden de logische regels heroverlopen.
        display(S, 1);
    }
    
    if (check(S) == 2)
        printf (" Er blijkt een fout in de sudoku te zitten.\n");
    else
    {
        printf("Sudoku opgeslaan in: %s: ",sudoku2);
        fp = fopen (sudoku2, "w");      // Open file, w = write
        for (i = 0; i < 81; ++i)
        {
            fprintf(fp, "%d", S[i][0]); // Print de sudoku in de file
            printf("%d", S[i][0]);
        }
        fclose(fp);                     // sluit de file
    }
    printf("\n\n");
    return 0;
}

void display(int S[][10], int dots)     // dots = 1 = met puntjes voor niet gekende waarden, dots = 0 = alle mogelijkheden tonen
{
    int i,j,k,l,m;
    for (i = 0; i < 3; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            for (k = 0; k < 3; ++k)
            {
                for (l = 0; l < 3; ++l)
                {
                    if(dots == 1)
                    {
                        if (S[27*i+9*j+3*k+l][0] != 0)
                            printf(" %d ", S[27*i+9*j+3*k+l][0]);
                        else
                            printf (" . ");
                    }
                    else
                    {
                        for (m = 1; m < 10; ++m)
                        {
                            if (S[27*i+9*j+3*k+l][m] != 0)
                                printf("%d", S[27*i+9*j+3*k+l][m]);
                            else
                                printf(" ");
                        }
                        printf("\t");
                    }
                }
                if (k != 2)
                    printf ("|");
                else 
                    printf ("\n");
            }
        }
        if (i != 2)
            if (dots == 1)
                printf ("---------+---------+---------\n");
            else
                printf ("------------------------------------------------+-----------------------------------------------+------------------------------------------------\n");
    }
    printf("\n");
}
void empty(int S[][10])                                 // Haalt de gekende waarden uit de mogelijkheden van locaties in dezelfde rij, kolom of vierkant
{
    int i,j,k,l,ki,kj;                                  
    for(i = 0; i < 9; ++i)                              // Rij
    {
        for(j = 0; j < 9; ++j)                          // Kolom
        {
            int number = S[9*i+j][0];
            if(number != 0)                             // Alleen waarden uit dezelfde rij/kolom/vierkant schrappen als er effectief een zeker waarde voor deze locatie bestaat
            {
                for(k = 0; k < 9; ++k)                  // Overloopt de rij/kolom
                {   
                    if(k != j)                          // Er mag niet gescrapt worden op de locatie van deze zeker waarde
                        S[9*i+k][number] = 0;           // Mogelijkheden uit de rij schrappen
                    if(k != i)                          // Er mag niet gescrapt worden op de locatie van deze zeker waarde
                        S[9*k+j][number] = 0;           // Mogelijkheden uit de kolom schrappen
                }
                
                if (i < 3)                              // Zoekt de beginwaarden van het vierkant
                    ki = 0;
                else if (i > 5)
                    ki = 6;
                else
                    ki = 3;
                if (j < 3)
                    kj = 0;
                else if (j > 5)
                    kj = 6;
                else
                    kj = 3;
                
                for(k = ki; k < ki+3; ++k)              // Rij van het vierkant
                {   
                    for(l = kj; l < kj+3; ++l)          // Kolom van het vierkant
                    {
                        if(k != i || l != j)            // Er mag niet gescrapt worden op de locatie van deze zeker waarde
                            S[9*k+l][number] = 0;       // Mogelijkheden uit het vierkant schrappen
                    }
                }
            }
        }
    }
}
int fill(int S[][10])                                   // Checkt op elke plaats of er genoeg mogelijkheden geschrapt zijn om met zekerheid te zeggen welke waarde er moet komen
{
    int finished = 1;

    int i,j,k,l,m,ki,kj;
    for(i = 0; i < 9; ++i)                                      // Rij
    {
        for(j = 0; j < 9; ++j)                                  // Kolom
        {
            int number = S[9*i+j][0];
            if(number == 0)                                     // Alleen naar een waarde voor deze locatie zoeken als er nog geen is
            {
                for(k = 1; k < 10; ++k)                         // Loopt alle mogelijk waarden af (1 tot 9)
                {
                    if (S[9*i+j][k] != 0)
                    {
                        int hitrow = 1,hitcolom = 1, hitsquare = 1, hitother = 1;
                        for(l = 0; l < 9; ++l)                  // Loopt de kolom/rij af
                        {   
                            if(l != j)                          // Op de locatie waar de waarde moet komen zal de waarde natuurlijk wel staan, dus deze mag overgeslagen worden
                                if (S[9*i+l][k] != 0)           // Zoekt de rij af of er een waarde is die alleen op (i,j) kan staan
                                    hitrow = 0;                 // Als er een bepaalde waarde k voorkomt in de rij l dan kan er geen is er geen hit
                            if(l != i)                          // Op de locatie waar de waarde moet komen zal de waarde natuurlijk wel staan, dus deze mag overgeslagen worden
                                if (S[9*l+j][k] != 0)           // Zoekt de kolom af of er een waarde is die alleen op (i,j) kan staan
                                    hitcolom = 0;               // Als er een bepaalde waarde k voorkomt in de kolom l dan kan er geen is er geen hit
                        }
                        
                        if (i < 3)                              // Zoekt de beginwaarden van het vierkant
                            ki = 0;
                        else if (i > 5)
                            ki = 6;
                        else
                            ki = 3;
                        if (j < 3)
                            kj = 0;
                        else if (j > 5)
                            kj = 6;
                        else
                            kj = 3;
                        
                        for(l = ki; l < ki+3; ++l)              // Rij van het vierkant
                        {   
                            for(m = kj; m < kj+3; ++m)          // Kolom van het vierkant
                            {
                                if(l != i || m != j)            // Op de locatie waar de waarde moet komen zal de waarde natuurlijk wel staan, dus deze mag overgeslagen worden
                                    if (S[9*l+m][k] != 0)       // Zoekt het vierkant af of er een waarde is die alleen op (i,j) kan staan
                                        hitsquare = 0;          // Als er een bepaalde waarde k voorkomt in dit vierkant dan kan er geen is er geen hit
                            }
                        }
                        for(l = 1; l < 10; ++l)                 // Indien alle mogelijkheden van deze locatie geschrapt zijn buiten 1, is er ook een hit
                        {                                       //  Dus als er wel een andere mogelijkheid is naast deze k is er geen hit
                            if (S[9*i+j][l] != k && S[9*i+j][l] != 0)
                                hitother = 0;
                        }
                        if(hitrow == 1 || hitcolom == 1 || hitsquare == 1 || hitother == 1) // Indien er iets een hit heeft dan is waarde k de enige mogelijkheid
                        {
                            finished = 0;                       // We zijn alleen klaar als we geen 1 waarde meer hebben kunnen aanpassen => nu nog niet klaar
                            for (l = 0; l < 10; ++l)            // Maak het getal definitief in de array door het op S[locatie][0] en op S[locatie][k] te zetten en alle andere plaatsen in de array 0 te maken
                            {
                                if (l == 0 || l == k)
                                    S[9*i+j][l] = k;
                                else
                                    S[9*i+j][l] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return finished;                                            // Indien 1 was er geen hit meer dus zal er niets meer aan de sudoku veranderen via de logische methode (of is de sudoku opgelost)
}
int check(int S[][10])                                          // Checkt of de array opgelost is, 0 voor niet opgelost, 1 voor opgelost en 2 voor een fout in de sudoku
{
    int finished = 1;
    
    int i,k,l;
    for(i = 0; i < 81; ++i)                                     // Locatie
    {
        int wrong = 1;
        for(k = 0; k < 10; ++k)                                 // overloopt alle mogelijkheden
        {
            if (k != 0 && S[i][k] != 0)                         // er zit geen fout in de sudoku zolang als er op elke locatie minstens 1 mogelijkheid bestaat 
                wrong = 0;                                      //  Als 2 locaties in dezelfde rij/kolom/vierkant dezelfde waarde hebben zullen zij elkaar schrappen en opgemerkt worden door bovenstaande if
            if (k != 0 && k != S[i][0] && S[i][k] != 0)         // De sudoku is nog niet klaar als er ergens nog meerdere mogelijkheden zijn
                finished = 0;
            
        }
        if (wrong == 1)
            return 2;
    }
    return finished;
}
int guess(int S[][10])                                          // Loopt alle mogelijkheden af voor de niet ingevulde plaatsen, bij elke ingevulde waarde worden de logische regels heroverlopen.
{                                                               // Elke guess() die recursief geopend wordt komt overeen met 1 locatie in de sudoku
    ++count;
    
    // -- BACKUP --                                             // Indien ergens een fout gevonden wordt de vorige backup terug gezet   
    
    int backupS[81][10];
    int x,y;
    for(x = 0; x < 81; ++x)
    {
        for(y = 0; y < 10; ++y)
        {
            backupS[x][y] = S[x][y];
        }
    }
    
    // -- NIET DEFINITIEVE LOCATIE ZOEKEN -- (location)
    
    int i,location = 81;
    while(i != 81 && location == 81)
    {
        if (S[i][0] == 0)
            location = i;
        ++i;
    }
    if (location == 81)                                         // Alle locaties zijn overlopen maar er is geen juiste oplossing gevonden = fout
        return 2; // FOUT
    
    // -- EERSTE MOGELIJKHEID VAN DE LOCATIE ZOEKEN -- (number)
    
    i = 0;
    int number = 0;
    while(i != 8 && number == 0)
    {
        ++i;
        if (S[location][i] != 0)
            number = i;
    }
    
    int finished = 2;                                           // Begin als fout
    while (finished == 2)                                       // Vorige mogelijkheid was fout => volgende mogelijkheid invullen
    {
        if (INFO == 1)
        {
            printf("location: %d; number: %d\n\n", location, number);
            Pause();
        }
        else                                                    // Progress bar
        {
            for (i = 0; i < count; ++i)
                printf("#");
            printf("\n");
        }
        // -- NUMBER OP LOCATION IN S STEKEN --
        
        S[location][0] = number;
        for (i = 1; i < 10; ++i)
        {
            if (i != number)
                S[location][i] = 0;
            else
                S[location][i] = number;
        }
    
        if (INFO == 1)
            display(S, 0);
        
        // -- LOGISCH OPLOSSEN -- 
        
        finished = 0;
        while(finished == 0)
        {
            empty(S);
            finished = fill(S);
        }
        finished = check(S);                                    // Check of de sudoku juist, fout of nog niet klaar is
        if (INFO == 1)
            display(S, 0);
        
        if (finished == 1)                                      // return 1
        {
            if (INFO == 1)
                printf("KLAAR\n");
            return 1; // JUIST
        }
        if (finished == 0)                                      // De sudoku kan opnieuw niet opgelost worden via logische bewerkingingen: volgende waarde gokken
        {
            if (INFO == 1)
                printf("Nog niet klaar\n");
            finished = guess(S);                                // Gaat recursief alles deptfirst aflopen tot een juiste oplossing gevonden is of alle mogelijkheden overlopen zijn                          
        }
        if (finished == 2)                                      // Indien de gok uit deze Guess of uit een kind van deze Guess fout was wordt de oorspronkelijke sudoku van bij het binnenkomen
        {                                                       //  van deze Guess herstelt en wordt daarin de volgende mogelijke waarde gegokt
            if (INFO == 1)
                printf("FOUT\n");
            
            // -- BACKUP HERSTELLEN --
            
            for(x = 0; x < 81; ++x)
            {
                for(y = 0; y < 10; ++y)
                {
                    S[x][y] = backupS[x][y];
                }
            }
            if (INFO == 1)
            {
                printf("S na backup\n");
                display(S, 0);
            }
            
            // -- VOLGENDE MOGELIJKHEID BEPALEN --
            
            i = number;
            number = 0;
            while(i != 9 && number == 0)                        // Het tellen begint vanaf de vorige gok zodat deze zeker niet opnieuw gegokt wordt
            {
                ++i;
                if (S[location][i] != 0)
                    number = i;
            }
            if (i == 9 && number != 9)                          // Alle mogelijheden zijn overlopen en alles is fout => er moet een fout hoger liggen => return dat er hoger een fout is
            {
                --count;
                return 2; // FOUT
            }
        }
    }
    return 1;                                                   // Klaar
}
void Pause()
{
    getchar();                                                  // Wacht tot er een char binnenkomt (enter telt als char) => bij enter doorgaan
}