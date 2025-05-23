#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"

void afficheStation( Tstation *station){
    if (station == NULL){
        printf("\nerreur station non allouee!\n");
    }
    else{
        if (station->arret_ou_troncon == ARRET){
            printf("\nSTATION idStation: %d, Station : %s, posX = %d, posY = %d", station->idStation, station->nomStation, station->posX, station->posY);
        }
        else{ //alors le noeud est un troneon
            printf("\n\nTRONCON idLigneBus: %d, temps: %d sec, distance e parcourir: %d metres", station->idLigneBus, station->coutTemps, station->coutDistance);
            afficheStation(station->depart);
            afficheStation(station->arrivee);
            printf("\nfin troneon\n");
        }
    }
}

void initStation( Tstation *station){
    srand( time( NULL ) );
    TDate date;
    date.annee = (rand() % 126) + 1900; //Date entre 1900 et 2025
    date.mois = rand() % 13;
    date.jour = rand() % 30;
    station->coutMaintenance = rand() % 91 + 10; 
    station->dateDerniereMaintenance = date;
}

// Getteurs

int getIdStation( Tstation *myStation){
    return myStation->idStation;
}

char *getNomStation( Tstation *myStation){
    return myStation->nomStation;
}

int getPosXStation( Tstation *myStation ){
    return myStation->posX;
}
int getPosYStation( Tstation *myStation ){
    return myStation->posY;
}

int getIdLigneTroncon(Tstation *myStation){
    //une station n'est pas liee e une ligne, seulement le troncon
    if (getTypeNoeud(myStation)==TRONCON)
        return myStation->idLigneBus;
    else{
        printf("\n(getIdLigneTroncon) Erreur algo, vous n etes pas sur un troncon");
        return -1;
    }
}

TypeNoeud getTypeNoeud(Tstation *myStation){
    return myStation->arret_ou_troncon;
}

int getCoutMaintenance(Tstation *myStation){
    return myStation->coutMaintenance;
}

TDate getDateDerniereMaintenance(Tstation *myStation){
    return myStation->dateDerniereMaintenance;
}



int getPosXBus( Tbus myBus ){
    return myBus->posXBus;
}
int getPosYBus( Tbus myBus ){
    return myBus->posYBus;
}
int getIdBus( Tbus myBus ){
    return myBus->idBus;
}
int getIdLigneActuelleDuBus( Tbus myBus ){
    return myBus->idLigneBusActuelle;
}

int getIdLigneBus(Tbus myBus){
    return myBus->idLigneBusActuelle;
}

TsensParcours getSensParcours(Tbus myBus){
    return myBus->sensParcours;
}

TlisteStation getActualStation( Tbus myBus ){
    return myBus->positionSurLaLigneDeBus;
}


// Setteurs

void setCoutMaintenance(Tstation *myStation, int cout){
    myStation->coutMaintenance = cout;
}

void setDateDerniereMaintenance(Tstation *myStation, TDate date){
    myStation->dateDerniereMaintenance = date;
}

void setActualStation( Tbus myBus, TlisteStation arrivalStation ){
    myBus->positionSurLaLigneDeBus = arrivalStation;
}

void setPosXBus(Tbus myBus, int newX){
    myBus->posXBus = newX;
}

void setPosYBus(Tbus myBus, int newY){
    myBus->posYBus = newY;
}

void setIdLigneBus(Tbus myBus, int idLigne){
    myBus->idLigneBusActuelle = idLigne;
}

void setSensParcours(Tbus myBus, TsensParcours sens ){
    myBus->sensParcours = sens;
}

void setPositionSurLaLigneDeBus( Tbus myBus, TlisteStation myStation){
    myBus->positionSurLaLigneDeBus = myStation;
}
