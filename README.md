# DNS Resolver - (VUT FIT ISA project)

## Popis programu
Tento program slouží pro zaslání DNS dotazu danému serveru a následné přijětí odpovědi, zpracování a vypsání na standardní výstup v uživatelsky přívětivé podobě.

## Použití
Program lze spustit následujícím způsobem: `./dns [-r] [-x] [-6] -s server [-p port] adresa`

Argumenty lze psát v libovolném pořadí, zde je jejich vysvětlení:
- `-r`: je požadovaná rekurze pro získání záznamu
- `-x`: je požadován reverzní dotaz
- `-6`: je požadován dotaz typu AAAA místo výchozího A
- `-s server`: IP adresa nebo doménové jméno serveru pro zaslání dotazu
- `-p` port: číslo portu, na který se má dotaz poslat, výchozí je 53
- `adresa`: dotazovaná adresa
    
Program poté vypíše na stdout informace o přijaté odpovědi. Nejprve vypíše, zda je odpověď autoritativní, zda byla zjištěna rekurzivně a zda byla odpověď zkrácena. Dále vypíše všechny sekce a záznamy obdržené v odpovědi.

## Seznam souborů
```
.
├── Makefile
├── manual.pdf
├── README
├── src
│   ├── arguments.c
│   ├── arguments.h
│   ├── error.c
│   ├── error.h
│   ├── main.c
│   ├── message.c
│   ├── message.h
│   ├── response.c
│   ├── response.h
│   ├── sending.c
│   └── sending.h
└── test
    ├── tests.c
    └── tests.py
```

