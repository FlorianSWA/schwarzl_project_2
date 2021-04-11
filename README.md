# NVS Projekt 2: Distance Vector Algorithmus
Dieses Programm ist eine lokale Simulation des Distance-Vector-Algorithmus. Es verwendet eine zufällig generierte Netzwerktopographie, wobei jeder Knoten durch einen Prozess, der auf einen bestimmten Port hört, dargestellt wird. Jeder Knoten kennt zu Begin nur seine direkten Nachbarn und erfährt den Weg zu weiteren Knoten über periodisch versendete Routing-Updates.

# Verwendung
Das Programm wird durch den Aufruf von `distance_vector_sim` über die Kommandozeile gestartet. Dieser Aufruf startet initial das Hauptprogramm namens `controller`. Danach werden automatisch die verlangte Anzahl an Netzwerkknoten als Kindprozess gestartet. Standardmäßig werden 6 Knoten mit 9 Verbindungen erstellt. Die Anzahl der Verbindungen ist die doppelte Anzahl der Knoten minus eins, mindestens gleich der Anzahl der Knoten. Eine Konfiguration ist sowohl über Kommandozeilemparameter als auch mittels JSON-Datei möglich. Die Standardwerte sind bei beiden Konfigurationsmöglichkeiten gleich.

## Parameter
Die folgende Tabelle enthält alle Parameter, welche über die Kommandozeile an das Programm übergeben werden können.
| Parametername | Datentyp        | Standardwert | Beschreibung |
| ------------- | --------------- | ------------ | ------------ |
|               | ganze Zahl >= 0 | 7            | Anzahl der Netzwerkknoten |
| -h, --help    |                 |              | Gibt die Hilfe aus |
| -l, --log     | boolean         | false        | Aktiviert Logging für Hauptprozess und alle Netzwerkknoten |
| -d, --debug   | boolean         | false        | Setzt Log-Level auf Debug. Benötigt --log |
| -f, --file    | string          |              | Konfigurationsdatei im JSON-Format |
| --failure     | boolean         | false        | Aktiviert die Simulation eines verbindungsausfalls |

Die JSON-Datei zur Konfiguration hat folgende Felder:
| Feldname | Datentyp        | Beschreibung |
| -------- | --------------- | ------------ |
| nodes    | ganze Zahl >= 0 | Anzahl der Netzwerkknoten |
| log      | boolean         | Aktiviert Logging für Hauptprozess und alle Netzwerkknoten |
| debug    | boolean         | Setzt Log-Level auf Debug. Benötigt --log |
| failure  | boolean         | Aktiviert die Simulation eines verbindungsausfalls |

# Algorithmus#
Der Distance-Vector-Algorithmus arbeitet nach dem Prinzip: Teile deinen Nachbarn mit, wie du die Welt siehts. Jeder Knoten hat eine Weiterleitungstabelle, in der für jeden ihm bekannten Knoten ein Eintrag ist. Dieser Eintrag beinhaltet den Zielknoten, den nächsten Knoten, über den dieser zu erreichen ist und die Distanz zum Ziel. Zu Beginn kennt jeder Knoten nur seine direkten Nachbarn. Regelmäßig sendet jeder Knoten an seine direkten Nachbarn Aktualisierungen aus, die seine Weiterleitungstabelle enthalten. Erhält ein Knoten nun so eine Aktualisierung, vergleicht er die erhaltenen Einträge mit jenen in seiner Weiterleitungstabelle. Ist der Knoten noch nicht enthalten, wird er mit einer um 1 erhöhten Distanz übernommen. Gibt es bereits einen Eintrag zu diesem Knoten werden die Distanzen verglichen, ist die um 1 erhöhte erhaltene Distanz kleiner als die bereits gespeicherte, wird der neue Eintrag übernommen und der Sender der Aktualisierung als nächster Knoten zum Erreichen des Ziels eingetragen.

## Verwendete Software
* [asio](https://think-async.com/Asio/)
* [spdlog](https://github.com/gabime/spdlog)
* [CLI11](https://github.com/CLIUtils/CLI11)
* [Google Protocol Buffer](https://developers.google.com/protocol-buffers/)
* [JSON  for modern C++](https://github.com/nlohmann/json)
