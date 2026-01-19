//VATSU-tech
/*
PREREQUIS: 
- 1 capteur ultason : pour mesurer la distatnce
- 1 moteur : pour controler le levie de frein
- 1 buzzer : pour le signalement sonor
- 2 bouton poussoir(ou systeme de contact): pour savoir quand le levier est a son etat normal
                                            et pouvoir reinitialiser tout le programme
- 3 realis de contatct : pour commander les broches du moteur et l'alimentation

FONCTIONNEMENT :  
  - le programme va continuelement tester si on est dans la plage ou le systeme doit 
    commencer a fonctionner, si il y est le programme vas tester et trouver la 
    plage dans la quel on se trouve, et en fonction de celle ci on peut
    lancer au conducteur un avertissement, freinner la voiture a interval definie ou
    freiner totalement et arreter la voiture.

  - apres l'execution du programme on peut appuyer sur le bouton poussoir (systeme de contact)
    pour reinitialiser le programme a zero

NOTA BENE : la position initial du relais doit etre pour le sens horlogique(abaisser le levier de frein)
            c'est seulement une fois ce relais enclacher que le moteur tourne dans le sens antihorlogique.

ce code a ete cree pour une version plus simple  du programme et pour etre plus comprehensible pour tout le monde
fait par l'Ir KATSUVA MALAMBO (VATSU)le 14 Janvier 2026
*/
#include <NewPing.h>  //inclure la bibliotheque pour gerer le capteur ultrason

#define zero 0     //definir la valeur zero pour ne pas avoir a se repeter
#define baud 9600  //definir la vitesse d'initialisation du moniteur serie

#define echo_pin 5        //definir le pin 5 pour la broche echo du capteur ultrason
#define trigger_pin 6     //definir le pin 6 pour la broche trigger du capteur ultrason
#define max_distance 200  //definir la distance max que le capteur peut mesurer en cm

#define pin_break_state 7     //definir la broche 7 pour savoir si le levier de frein est a son etat initial ou non
#define pin_commande_reset 8  //definir la broche 8 pour reinitialiser tout le systeme
#define BUZZER 9              //definir la broche 9 pour commander le buzzer

#define relais_commande_un 3    //definir la broche 3 comme commande du relais de la broche un du moteur
#define relais_alimentation 2   //definir la broche 2 comme commande du relais qui controle l'alimentation du moteur
#define relais_commande_deux 4  //definir la broche 4 comme commande du relais de la broche deux du moteur

#define distance_avertissement 50  //definir la distance(en cm) a laquel l'utilisateur doit avoir un avertissement
#define distance_freinage_step 30  //definir la distance(en cm) a laquel la voiture freinne selon un interval definie
#define distance_freinage_stop 10  //definir la distance(en cm) a laquel les freins doivent etre totalement activer pour arreter de force le vehicule

NewPing capteur_ultrason(trigger_pin, echo_pin, max_distance);  //cree l'instance du cateur ultrason

bool avertissement = false;  //declarer la variable booleenne avertissement qui permet d'activer le programme d'avertissement
bool freinage_step = false;  //declarer la variable booleenne freinage_step qui permet d'activer le programme de freinage par interval
bool freinage_stop = false;  //declarer la variable booleennne freinage_stop qui permet d'activer le programme qui arrete le vehicule de force

bool reset_systeme = false;

bool bouton_frein_appui = false;
bool bouton_reset_systeme = false;

unsigned int distance = 0;

void setup() {
  Serial.begin(baud);

  pinMode(pin_break_state, INPUT);
  pinMode(pin_commande_reset, INPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(relais_commande_un, OUTPUT);
  pinMode(relais_alimentation, OUTPUT);
  pinMode(relais_commande_deux, OUTPUT);
}

void loop() {
  bouton_frein_appui = digitalRead(pin_break_state);
  bouton_reset_systeme = digitalRead(pin_commande_reset);

  distance = capteur_ultrason.ping_cm();
  Serial.println(distance);
  delay(50);

  if (bouton_frein_appui == true) {
    digitalWrite(BUZZER, false);
    digitalWrite(relais_alimentation, false);
  } else {

    if ((distance <= distance_avertissement) && (distance > 0) && (distance != 0)) {
      if ((distance <= distance_avertissement) && (distance > distance_freinage_step)) {
        digitalWrite(relais_alimentation, false);                                       
        digitalWrite(BUZZER, true);
        delay(1000);
        digitalWrite(BUZZER, false);
        delay(1000);
      } else if ((distance <= distance_freinage_step) && (distance > distance_freinage_stop)) {
        digitalWrite(BUZZER, true);
        digitalWrite(relais_alimentation, true);
        digitalWrite(relais_commande_un, true);
        digitalWrite(relais_commande_deux, true);
        delay(500);
        digitalWrite(BUZZER, false);
        digitalWrite(relais_commande_un, false);
        digitalWrite(relais_commande_deux, false);
        delay(500);

      } else if ((distance <= distance_freinage_stop) && (distance > zero)) {
        digitalWrite(BUZZER, true);
        digitalWrite(relais_alimentation, true);
        digitalWrite(relais_commande_un, false);
        digitalWrite(relais_commande_deux, false);
      }

    } else {
      digitalWrite(BUZZER, false);
      digitalWrite(relais_commande_un, false);
      digitalWrite(relais_alimentation, false);
      digitalWrite(relais_commande_deux, false);
    }

    if (bouton_reset_systeme == true) {
      if (bouton_frein_appui == false) {
        digitalWrite(relais_alimentation, true);
        digitalWrite(relais_commande_un, true);
        digitalWrite(relais_commande_deux, true);
      } else {
        digitalWrite(relais_commande_un, false);
        digitalWrite(relais_alimentation, false);
        digitalWrite(relais_commande_deux, false);
      }
    }
  }
}
