# Cahier des charges du systeme de gestion du trafic au rond-point Ngaba

## 1. Objet du document

Ce cahier des charges definit les attentes fonctionnelles et non fonctionnelles du
systeme de regulation du trafic au rond-point Ngaba. Il sert de reference pour
l'analyse, la conception, l'implementation et la validation du projet.

## 2. Contexte

Le rond-point Ngaba est un carrefour strategique de Kinshasa. Il connecte plusieurs
axes importants, notamment l'avenue de l'Universite et l'axe By-Pass. Le trafic y est
dense, variable et heterogene. Les embouteillages, les conflits de priorite et les
temps d'attente eleves rendent necessaire une regulation plus structuree.

Le projet vise la conception d'un systeme de controle de trafic capable de simuler un
pilotage par feux, de representer les files d'attente, d'alterner des phases de
circulation compatibles et d'offrir un support de supervision visuelle.

## 3. Objectif general

Concevoir et simuler un systeme de gestion du trafic routier au rond-point Ngaba afin
d'ameliorer la fluidite, reduire les temps d'attente et garantir la securite des
usagers.

## 4. Objectifs specifiques

- observer ou estimer l'etat du trafic sur chaque branche du rond-point,
- commander des feux tricolores selon une logique de phases,
- adapter la duree des phases aux charges detectees,
- separer les mouvements incompatibles,
- permettre une visualisation claire de l'etat courant,
- fournir un prototype de simulation pedagogique exploitable pour la soutenance.

## 5. Parties prenantes

- Conducteurs: attendent une meilleure fluidite et moins d'attente.
- Operateur de supervision: observe l'etat des feux, des files et des phases.
- Service technique: regle les parametres et maintient le dispositif.
- Autorite de voirie: valide la strategie de regulation retenue.
- Equipe projet: analyse, concoit, implemente et verifie le systeme.

## 6. Perimetre

### 6.1 Inclus dans le projet

- simulation des arrivees de vehicules,
- representation du trafic par branche et par mouvement,
- logique de changement de phase,
- adaptation simple des durees de vert,
- visualisation graphique du carrefour,
- documentation de specification et de conception.

### 6.2 Exclu du projet

- connexion a des feux physiques reels,
- acquisition capteur reelle sur site,
- coordination inter-carrefours,
- certification industrielle temps reel dur,
- integration de protocoles V2X ou IoT reels.

## 7. Description generale du besoin

Le systeme doit reguler les acces au rond-point au moyen de feux tricolores. Il doit
tenir compte du niveau de trafic observe ou simule sur les axes principaux. La
strategie retenue repose sur une alternance de phases vertes, oranges et tout rouge
de securite. Le systeme doit rester lisible, demonstratif et facile a expliquer dans
un contexte academique.

## 8. Exigences fonctionnelles

- EFG-01: Le systeme doit modeliser les quatre branches principales du rond-point.
- EFG-02: Le systeme doit representer les mouvements autorises depuis chaque branche.
- EFG-03: Le systeme doit estimer les files de vehicules en attente.
- EFG-04: Le systeme doit commander les feux selon des phases compatibles.
- EFG-05: Le systeme doit integrer des etats `VERT`, `ORANGE` et `ROUGE`.
- EFG-06: Le systeme doit inserer une phase de securite tout rouge entre deux verts incompatibles.
- EFG-07: Le systeme doit adapter la duree d'une phase en cas de congestion.
- EFG-08: Le systeme doit mettre a jour le trafic au fil du temps.
- EFG-09: Le systeme doit afficher la scene du rond-point de maniere graphique.
- EFG-10: Le systeme doit afficher la phase active, le temps restant et le cycle.
- EFG-11: Le systeme doit permettre un mode boucle pour rejouer la simulation.
- EFG-12: Le systeme doit permettre l'inspection visuelle du carrefour avec zoom et dezoom.

## 9. Exigences non fonctionnelles

- ENF-01: Le systeme doit etre comprehensible par une equipe etudiante.
- ENF-02: Le code doit etre modulaire et separe entre logique metier, simulation et affichage.
- ENF-03: Le systeme doit fonctionner sur une machine standard sans infrastructure specifique.
- ENF-04: La visualisation doit etre suffisamment lisible pour une demonstration.
- ENF-05: Les parametres critiques doivent pouvoir etre ajustes facilement.
- ENF-06: Le prototype doit rester stable sur plusieurs cycles de simulation.

## 10. Exigences de surete

- ES-01: Deux flux incompatibles ne doivent pas etre verts simultanement.
- ES-02: Toute fin de vert doit passer par une phase orange.
- ES-03: Une transition de securite tout rouge doit separer deux familles de mouvements.
- ES-04: En cas d'etat incoherent, le systeme doit revenir a un etat sur.
- ES-05: La supervision doit permettre de constater rapidement l'etat des feux.

## 11. Hypotheses de travail

- le rond-point est ramene a quatre branches principales,
- le trafic est simule et non mesure sur le terrain,
- les capteurs sont representes par une estimation logique des files,
- les vehicules sont geres sous forme agregee avec animation graphique,
- C++ avec Qt Widgets est utilise ici comme socle de simulation pedagogique.

## 12. Contraintes

- contrainte academique de lisibilite et de justification methodologique,
- contrainte de temps de realisation limitee,
- absence de donnees terrain fiables integrees au prototype,
- absence de plateforme temps reel embarquee cible.

## 13. Critieres d'acceptation

Le projet sera considere recevable si:

- les besoins et contraintes sont clairement identifies,
- la specification montre une logique de regulation coherente,
- la conception justifie la structuration du systeme,
- le simulateur s'execute et illustre le comportement attendu,
- les limites du prototype sont expliquees explicitement.

## 14. Livrables attendus

- un cahier des charges,
- une specification du systeme selon SA-RT,
- une conception du systeme selon DARTS,
- un simulateur logiciel du systeme.

## 15. Risques identifies

- simplification excessive du comportement reel du trafic,
- absence de calibration sur observations terrain,
- ecart entre simulation logique et contraintes d'un vrai systeme temps reel,
- manque de tests automatiques si le projet n'est pas complete.

## 16. Conclusion

Ce cahier des charges fixe un cadre coherent pour un projet pedagogique de gestion du
trafic au rond-point Ngaba. Il oriente la specification detaillee, la conception
temps reel et la realisation du simulateur tout en rappelant les limites actuelles du
prototype.
