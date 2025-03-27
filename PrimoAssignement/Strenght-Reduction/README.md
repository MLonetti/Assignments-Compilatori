# Strenght Reduction

mettere il contenuto di questo readme nella documentazione della directory del passo finale, in cui metteremo tutto. Leggi lineeguida per capire il piano Cama

-------------------------------------------------------------------------------------------------------------------------------------------------------------------

nel seguente passo di ottimizzazione applicheremo la Strenght Reduction
....

x * 8 -> shl x, 3

x * 15 -> shl (x,4) - x -> è come fare  x * 16 - x

x * 18 -> rimane x * 18, non conviene applicare la shift, non sarebbe ottimizzazione 
