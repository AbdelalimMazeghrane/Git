# Git
      Introduction


Git est un outil de contrôle de version très populaire utilisé par les développeurs de logiciels pour
suivre l'évolution des fichiers de code source d'un projet. Il permet notamment de conserver un
historique des modifications effectuées sur les fichiers, de travailler en équipe sur un même projet,
de gérer les conflits éventuels entre les modifications, et de revenir à une version antérieure du
projet en cas de besoin.
Dans le cadre de ce projet, nous avons cherché à reproduire les principales fonctionnalités de Git en
utilisant le langage C. Nous avons travaillé en équipe pour développer un système de gestion de
version qui permet de suivre les modifications effectuées sur les fichiers d'un projet, de créer des
branches pour travailler sur des versions alternatives, de fusionner des branches pour intégrer les
modifications, et de récupérer des versions antérieures du projet.
        
        Architecture globale du projet


Le projet est organisé en plusieurs fichiers, chacun ayant un rôle spécifique dans l'architecture
globale.
Le fichier "Makefile" est un script qui permet de compiler l'ensemble du projet. Il définit les
dépendances entre les différents fichiers et les commandes nécessaires à leur compilation.
Le fichier "commit.h" définit la structure des objets "commit", qui représentent une version du projet
dans notre système de gestion de version de fichier ou des projets. Il contient également les
déclarations des fonctions nécessaires à la gestion des commits.
Le fichier "commit.c" contient les implémentations des fonctions déclarées dans "commit.h".
Le fichier "hashtable.h" définit la structure des tables de hachage utilisées pour stocker les commits
dans notre système.
Le fichier "hashtable.c" contient les implémentations des fonctions nécessaires à la gestion des tables
de hachage.
Le fichier "list.h" définit la structure des listes chaînées utilisées pour stocker les fichiers d'un commit.
Le fichier "list.c" contient les implémentations des fonctions nécessaires à la gestion des listes
chaînées.
Le fichier "utilitaire.h" contient les déclarations des fonctions utilitaires utilisées dans le projet.
Le fichier "utilitaire.c" contient les implémentations des fonctions utilitaires.
Le fichier "workfile.h" définit la structure des objets "workfile", qui représentent les fichiers du projet
en cours de modification.
Le fichier "workfile.c" contient les implémentations des fonctions nécessaires à la gestion des
workfiles.
Le fichier "worktree.h" définit la structure de l'objet "worktree", qui représente l'état actuel du projet
dans notre système.Le fichier "worktree.c" contient les implémentations des fonctions nécessaires à la gestion du
worktree.
Enfin, le fichier "main.c" est le point d'entrée du programme. Il contient la fonction main, qui initialise
le système, traite les commandes entrées par l'utilisateur et affiche les résultats.
Le fichier "valgrind-out.txt" est un fichier de sortie généré par l'outil de débogage Valgrind, qui
permet de détecter des erreurs de mémoire ou des fuites de mémoire dans le code.


      Cas d’utilisation.


Voici une description des différentes fonctionnalités offertes par le projet :
Init : Cette fonction permet d'initialiser un nouveau dépôt Git dans un répertoire. Elle crée le dossier
".git" contenant les fichiers nécessaires pour le suivi des versions.
List-refs : Cette fonction permet d'afficher la liste des références (branches, tags, etc.) présentes dans
le dépôt.
Create-ref : Cette fonction permet de créer une nouvelle référence (une branche par exemple) à
partir d'un commit donné.
Delete-ref : Cette fonction permet de supprimer une référence existante (une branche par exemple).
Add : Cette fonction permet d'ajouter un fichier au suivi de version.
Clear-add : Cette fonction permet de supprimer un fichier du suivi de version.
List-add : Cette fonction permet d'afficher la liste des fichiers ajoutés au suivi de version.
Commit : Cette fonction permet de créer un nouveau commit à partir des fichiers ajoutés au suivi de
version. Le commit contient un message décrivant les modifications apportées.
Get-current-branch : Cette fonction permet d'obtenir le nom de la branche courante.
Branch : Cette fonction permet de créer une nouvelle branche à partir de la branche courante.Branch-
print : Cette fonction permet d'afficher la liste des branches présentes dans le dépôt.
Checkout-branch : Cette fonction permet de changer la branche courante pour une autre branche
existante. Cela permet de travailler sur une version alternative du projet.
Chaque fonctionnalité permet de reproduire une partie des fonctionnalités de Git. Ainsi, le projet
permet de suivre l'historique des modifications effectuées sur un projet, de travailler en équipe sur
un même projet en utilisant des branches, et de revenir à des versions antérieures du
projet si besoin.
Toutes les fonctionnalités du projet ont été testées et déboguées afin de garantir l'absence de fuites
de mémoire. Pour cela, l'outil de débogage Valgrind a été utilisé avec la commande "valgrind --leak-
check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt" pour vérifier que le
programme n'utilise pas de mémoire non allouée ou libérée. La ligne de code de Valgrind on a trouvé
dans la documentation valgrind sur internet.
De plus, l'outil de débogage GDB a été utilisé pour traquer et corriger les erreurs
d'exécution dans le code.En somme, toutes les fonctionnalités du projet ont été rigoureusement testées et déboguées pour
garantir la qualité et la fiabilité du code, ainsi que la sécurité de l'application.
      
      
      Construction du projet.


Voici les différentes étapes pour compiler le projet:
1 -Ouvrir un terminal shell et se positionner dans le répertoire du projet
2 -Exécuter la commande make (ou make clean après une recompilation precedente)
Le nom de l'exécutable généré est my_git, comme indiqué dans le fichier Makefile par la variable
EXEC. Lorsque la commande make est exécutée, le programme est compilé et l'exécutable est généré
avec ce nom. Le fichier exécutable sera donc nommé my_git.


    Exemple d’utilisation.


Afin d'executer le programme il faudra commencer avec la fonction init: comme ceci ./my_git init
Cela va initialiser le répertoire courant en créant un dossier ".add"
nous pourrions après ajouter un fichier à la "staging area" en le créant et en faisant ./my_git add
mon_fichier_texte.txt
Par la suite on peut commit ce fichier la en utilisant ./my_git commit -m "premier commit"
Voici d'autres exemples d'utilisation des autres fonctionnalités :
Pour afficher les références existantes : ./my_git list-refs
Pour créer une nouvelle référence : ./my_git create-ref <nom de la référence> <ID du commit>
Pour supprimer une référence existante : ./my_git delete-ref <nom de la référence>
Pour ajouter des fichiers à l'aire de staging : ./my_git add <chemin vers le fichier>
Pour supprimer tous les fichiers de l'aire de staging : ./my_git clear-add
Pour afficher les fichiers présents dans l'aire de staging : ./my_git list-add
Pour créer un nouveau commit : ./my_git commit <nom de la référence> -m <message du commit>
(le message du commit est optionnel)
Pour afficher le nom de la branche actuelle : ./my_git get-current-branch
Pour créer une nouvelle branche : ./my_git branch <nom de la nouvelle branche>
Pour afficher les commits sur une branche : ./my_git branch-print <nom de la branche>
Pour passer à une autre branche : ./my_git checkout-branch <nom de la branche>
Pour passer à un commit spécifique : ./my_git checkout-commit <ID du commit>
        
  
        Conclusion

  
  En conclusion, ce projet de développement d'un système de contrôle de version "my_git" a permis de
mettre en pratique des concepts clés en programmation tels que la manipulation de fichiers,
l'utilisation de structures de données, la gestion de la mémoire et la gestion des erreurs. Le projet a
également permis de comprendre les principes fondamentaux du contrôle de version et de
développer des compétences en matière de collaboration et de travail en équipe.
Bien que ce projet ait pu être amélioré en ajoutant des fonctionnalités supplémentaires telles que la
fusion de branches et la gestion de conflits, il a permis d'acquérir une solide compréhension des
concepts clés de la gestion de version et de la programmation en général.
