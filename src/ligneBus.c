#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "types.h"
#include "ligneBus.h"
#include "listeDouble.h"

int getDistStations(Tstation station1, Tstation station2)
{
    return abs((int)sqrt(pow(station2.posX - station1.posX, 2) + pow(station2.posY - station1.posY, 2)));
}

Tbus creeBus(int idBus, TlisteStation start)
{
    Tbus myBus = (Tbus)malloc(sizeof(Typebus));
    myBus->idBus = idBus;
    busSurStation(myBus, start, depart_vers_arrivee);
    busSurLigneX(myBus, getIdLigneTroncon(getPtrData(getNextTroncon(start))));
    return myBus;
}

Tstation *creeArret(int posX, int posY, char *nomStation, int idStation)
{
    Tstation *newStation = (Tstation *)malloc(sizeof(Tstation));

    newStation->arret_ou_troncon = ARRET;

    // champs utiles si ARRET
    newStation->posX = posX;
    newStation->posY = posY;
    strcpy(newStation->nomStation, nomStation);
    newStation->idStation = idStation;

    // champs non utiles si ARRET
    newStation->depart = NULL;
    newStation->arrivee = NULL;
    newStation->coutTemps = 0;
    newStation->coutDistance = 0;
    newStation->idLigneBus = -1; // id inexistant
    return newStation;
}

Tstation *creeTroncon(int idLigneBus, Tstation *depart, Tstation *arrivee, int coutTemps, int coutDistance)
{
    Tstation *newStation = (Tstation *)malloc(sizeof(Tstation));

    newStation->arret_ou_troncon = TRONCON;

    // champs non utiles si TRONCON
    newStation->posX = 0;
    newStation->posY = 0;
    strcpy(newStation->nomStation, "");
    newStation->idStation = -1; // id inexistant

    // champs utiles si TRONCON
    newStation->idLigneBus = idLigneBus;
    newStation->depart = depart;
    newStation->arrivee = arrivee;
    newStation->coutTemps = coutTemps;
    newStation->coutDistance = coutDistance;
    return newStation;
}

int compterNombreLignes(char *nom_fichier)
{
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL)
    {
        printf("Erreur d'ouverture du fichier : %s\n", nom_fichier);
        return -1;
    }

    char ligne[100];
    int nombreLignes = 0;

    while (fgets(ligne, sizeof(ligne), fichier))
    {
        if (strncmp(ligne, "Ligne:", 6) == 0)
        {
            nombreLignes++;
        }
    }

    fclose(fichier);
    return nombreLignes;
}

TlisteStation *chargerLignes(char *nom_fichier, int *nbLignes)
{
    FILE *fichier = fopen(nom_fichier, "r");
    TlisteStation newLigne;

    *nbLignes = compterNombreLignes(nom_fichier);
    TlisteStation *lignes = malloc(sizeof(TlisteStation) * *nbLignes);
    int ligne_actuelle = 0;

    if (fichier == NULL)
    {
        printf("Erreur d'ouverture du fichier ! | Nom fichier %s\n", nom_fichier);
        return lignes;
    }

    int id_arret, id_ligne, x, y;
    char nom[50];

    // Tant qu'il y a des lignes (ex: "Ligne: 1") dans le fichier
    while (fscanf(fichier, "Ligne: %d\n", &id_ligne) == 1)
    {
        // On ititialise une nouvelle ligne
        initListe(&newLigne);

        // Ajout de la première station
        fscanf(fichier, "%d;%49[^;];%d;%d\n", &id_arret, nom, &x, &y);
        Tstation *dep = creeArret(x, y, nom, id_arret);
        newLigne = ajoutEnFin(newLigne, dep);

        // Ajout des autres stations avec les troncons
        while (fscanf(fichier, "%d;%49[^;];%d;%d\n", &id_arret, nom, &x, &y) == 4)
        {
            Tstation *arr = creeArret(x, y, nom, id_arret);
            int dist = getDistStations(*dep, *arr);
            Tstation *troncon = creeTroncon(id_arret, dep, arr, dist, dist);
            newLigne = ajoutEnFin(newLigne, troncon);
            newLigne = ajoutEnFin(newLigne, arr);
            dep = arr;
        }

        lignes[ligne_actuelle] = newLigne;
        ligne_actuelle++;
    }

    return lignes;
}

TlisteStation *creeLignesDeBus(int *nbLignes)
{
    return chargerLignes("data/Stations_et_lignesDeBus.data", nbLignes);
}

void afficheConsoleLigneBus(TlisteStation l)
{
    afficheListe(l);
}

TlisteStation getNextStation(TlisteStation l)
{
    if (ligneBusVide(l))
        return NULL;
    else
    {
        if (getTypeNoeud(getPtrData(l)) == ARRET)
        {
            if (ligneBusVide(getNextCell(l)))
                return NULL; // nous sommes sur un terminus
            else
                return getNextCell(getNextCell(l)); // le prochain arret est dans 2 cellules dans la liste, la cellule suivaante �tant un TRONCON
        }
        else
            return getNextCell(l); // si on est sur un TRONCON alors la prochaine cellule est la prochaine station (ARRET)
    }
}

TlisteStation getNextTroncon(TlisteStation l)
{
    if (ligneBusVide(l))
        return NULL;
    else
    {
        if (getTypeNoeud(getPtrData(l)) == ARRET)
        {
            return getNextCell(l); // la prochaine cellule est un TRONCON, peut �tre �gale � NULL si on est sur un terminus
        }
        else
        {
            printf("\n(getNextTroncon) erreur algo, on est deja sur un troncon");
            return l; // on retourne le troncon actuel
        }
    }
}

TlisteStation getPreviousStation(TlisteStation l)
{
    if (ligneBusVide(l))
        return NULL;
    else
    {
        if (getTypeNoeud(getPtrData(l)) == ARRET)
        {
            if (ligneBusVide(getPrevCell(l)))
                return NULL;                    // nous sommes sur un terminus
            return getPrevCell(getPrevCell(l)); // le prochain arret est dans 2 cellules dans la liste, la cellule suivaante �tant un TRONCON
        }
        else
            return getPrevCell(l); // si on est sur un TRONCON alors la prochaine cellule est la prochaine station (ARRET)
    }
}

bool ligneBusVide(TlisteStation l)
{
    return listeVide(l);
}

int getPosXListeStation(TlisteStation myStationInListe)
{
    return getPosXStation(getPtrData(myStationInListe));
}

int getPosYListeStation(TlisteStation myStationInListe)
{
    return getPosYStation(getPtrData(myStationInListe));
}

void deplaceBus(Tbus myBus, TsensParcours sens_deplacement, int *incXSprite, int *incYSprite)
{
    TlisteStation dest;
    int xdep, ydep, xarr, yarr, pas;
    float ratio;
    pas = 2; // �quivalent au parametre d'erreur dans le trac� de Segment de Bresenham
    // https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_segment_de_Bresenham

    if (sens_deplacement == depart_vers_arrivee)
        dest = getNextStation(getActualStation(myBus));
    else
        dest = getPreviousStation(getActualStation(myBus));

    // par d�faut: pas de d�placement du bus
    *incXSprite = 0;
    *incYSprite = 0;

    // si un prochain arret existe?
    if (ligneBusVide(dest))
    {
        // printf("\nBus id%d est au terminus de la ligne %d sur la station id=%d %s", getIdBus(myBus), getIdLigneActuelleDuBus(myBus),getIdStation(getPtrData(getActualStation(myBus))),getNomStation(getPtrData(getActualStation(myBus))));
    }
    else // s'il existe:
    {
        // soit on est arriv�, soit on est en cours de trajet
        // r�cup�ration des coordonn�es
        xdep = getPosXBus(myBus);
        ydep = getPosYBus(myBus);
        xarr = getPosXListeStation(dest);
        yarr = getPosYListeStation(dest);

        // sommes-nous arriv�s?
        if ((abs(xdep - xarr) <= pas) && (abs(ydep - yarr) <= pas))
        {
            // alors mise � jour de la station actuelle du bus
            setActualStation(myBus, dest);
            printf("\nLe Bus id%d arrive sur la station id%d %s", getIdBus(myBus), getIdStation(getPtrData(getActualStation(myBus))), getNomStation(getPtrData(getActualStation(myBus))));
        }
        else
        {
            // sinon on calcule les offsets de d�placements pour la boucle main qui r�alise les affichages
            ratio = (float)(abs(yarr - ydep)) / (float)(abs(xarr - xdep)); // div euclydienne

            // algo de Bresenham "simplifi� de fa�on ad hoc" et non optimis� (� cause des floats)
            if (xarr > xdep)
                *incXSprite = pas;
            else if (xarr < xdep)
                *incXSprite = -pas;

            if (yarr > ydep)
                *incYSprite = (int)((float)(pas)*ratio); // pour un d�placement proportionnel en Y selon Y/X
            else if (yarr < ydep)
                *incYSprite = (int)((float)(-pas) * ratio);

            // remmarque: si xarr==ydep alors *incXSprite reste � 0, idem en Y

            // maj des coord du bus
            setPosXBus(myBus, getPosXBus(myBus) + *incXSprite);
            setPosYBus(myBus, getPosYBus(myBus) + *incYSprite);
        }
    }
}

// Setteurs
void setActualStation(Tbus myBus, TlisteStation arrivalStation);
void setPosXBus(Tbus myBus, int newX);
void setPosYBus(Tbus myBus, int newY);
void setIdLigneBus(Tbus myBus, int idLigne);
void setSensParcours(Tbus myBus, TsensParcours sens);

void setPositionSurLaLigneDeBus(Tbus myBus, TlisteStation myStation);

void busSurStation(Tbus myBus, TlisteStation myStation, TsensParcours sens)
{

    // Exemple encapsulation: on a pr�f�r� les fonctions setteurs � un acc�s direct aux champs

    setPositionSurLaLigneDeBus(myBus, myStation);
    // myBus->positionSurLaLigneDeBus = myStation;  //interdit

    setSensParcours(myBus, sens);
    // myBus->sensParcours = sens;

    setPosXBus(myBus, getPosXListeStation(myStation));
    // myBus->posXBus = getPosXListeStation( myStation );

    setPosYBus(myBus, getPosYListeStation(myStation));
    // myBus->posYBus = getPosYListeStation( myStation );

    // idem : on passe ci-dessous par les getteurs et non par un acc�s direct via les champs
    printf("\nBus id%d est en (x = %d, y = %d) sur la ligne %d, station %s", getIdBus(myBus), getPosXBus(myBus), getPosYBus(myBus), getIdLigneActuelleDuBus(myBus), getNomStation(getPtrData(myStation)));
}

void busSurLigneX(Tbus myBus, int idLigneX)
{
    setIdLigneBus(myBus, idLigneX);
}

TlisteStation stationSuivantePourLeBus(Tbus myBus)
{
    TlisteStation prochaineStation;
    if (getSensParcours(myBus) == depart_vers_arrivee)
        prochaineStation = getNextStation(getActualStation(myBus));
    else
        prochaineStation = getPreviousStation(getActualStation(myBus));
    return prochaineStation;
}

void afficheCoordonneesBus(Tbus myBus)
{
    char *nomProchaineStation;
    TlisteStation prochaineStation = stationSuivantePourLeBus(myBus);

    if (ligneBusVide(prochaineStation))
        printf("\nBus id: %d est en (x = %d, y = %d) ligne %d, derniere station %s (Terminus)", getIdBus(myBus), getPosXBus(myBus), getPosYBus(myBus), getIdLigneActuelleDuBus(myBus), getNomStation(getPtrData(getActualStation(myBus))));
    else
    {
        nomProchaineStation = getNomStation(getPtrData(prochaineStation));
        printf("\nBus id: %d est en (x = %d, y = %d) ligne %d, derniere station %s, prochaine station %s", getIdBus(myBus), getPosXBus(myBus), getPosYBus(myBus), getIdLigneActuelleDuBus(myBus), getNomStation(getPtrData(getActualStation(myBus))), nomProchaineStation);
    }
}

// Cr�er ci-dessous vos fonctions

void ajouterStationsEtTroncons(TlisteStation *newLigne, TlisteStation ligne)
{
    TlisteStation currentStation = ligne;
    Tstation *troncon;
    int dist;

    while (currentStation != NULL)
    {
        if (getTypeNoeud(getPtrData(currentStation)) == ARRET)
        {
            *newLigne = ajoutEnFin(*newLigne, getPtrData(currentStation));
        }
        else if (getTypeNoeud(getPtrData(currentStation)) == TRONCON)
        {
            dist = getDistStations(*getPtrData(currentStation), *getPtrData(getNextCell(currentStation)));
            troncon = creeTroncon(
                getIdLigneTroncon(getPtrData(currentStation)),
                getPtrData(getPrevCell(currentStation)),
                getPtrData(getNextCell(currentStation)),
                dist,
                dist);
            *newLigne = ajoutEnFin(*newLigne, troncon);
        }
        currentStation = getNextCell(currentStation);
    }
}

TlisteStation jonctionLigneDeBus(TlisteStation ligne1, TlisteStation ligne2)
{
    TlisteStation newLigne;
    Tstation *troncon;
    int dist;
    TlisteStation lastStation = getLastCell(ligne1);

    initListe(&newLigne);

    ajouterStationsEtTroncons(&newLigne, ligne1);

    dist = getDistStations(*getPtrData(lastStation), *getPtrData(ligne2));
    troncon = creeTroncon(
        (-1),
        getPtrData(lastStation),
        getPtrData(ligne2),
        dist,
        dist);
    newLigne = ajoutEnFin(newLigne, troncon);

    ajouterStationsEtTroncons(&newLigne, ligne2);
    return newLigne;
}

void supprimerStation(TlisteStation *ligne, int idStation)
{
    TlisteStation current = *ligne;
    Tstation *station;

    while (current != NULL)
    {
        station = getPtrData(current);

        if (getIdStation(station) == idStation)
        {

            if (current->prec == NULL && current->suiv == NULL)
            { // Si la liste ne contient qu'un seul élément
                *ligne = NULL;
            };

            if (current->prec == NULL && current->suiv != NULL)
            { // Au début de la liste
                *ligne = current->suiv->suiv;
                (*ligne)->prec = NULL;
            };

            if (current->prec != NULL && current->suiv != NULL)
            { // Au mileu de la liste
                current->prec->prec->suiv = current->suiv;
                current->suiv->suiv->prec = current->prec;
            };

            if (current->prec != NULL && current->suiv == NULL)
            { // A la fin de la liste
                current->prec->prec->suiv = NULL;
            };

            printf("Station ID %d supprimee de la ligne.\n", idStation);
            return;
        }
        current = current->suiv;
    }
    printf("Station ID %d introuvable dans la ligne.\n", idStation);
}

void circulaire(TlisteStation *ligne) {
    TlisteStation lastStation = getLastCell(*ligne);
    TlisteStation nouv;
    Tstation *troncon;
    
    int dist;


    dist = getDistStations(*getPtrData(lastStation), *getPtrData(*ligne));
    troncon = creeTroncon(
        (-1),
        getPtrData(lastStation),
        getPtrData(*ligne),
        dist,
        dist
    );

    nouv = (T_liste)malloc(sizeof(struct T_cell));

    nouv->pdata = troncon;

    nouv->suiv = *ligne;
    nouv->prec = lastStation;

    lastStation->suiv = nouv;
    (*ligne)->prec = nouv;
}