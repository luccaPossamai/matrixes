# Matrixes
A project all based on making the analysis of matrixes easier. The project make use of raylib library to render all the UI.

## Exports
Export snapshots of the matrix as eps file up to a 'very large number'. 

<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/fad614de-1548-4c1d-b941-b4bc4d99fa29" />
<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/62e3b245-9fb8-47be-8478-b9ac6c890c13" />


## Include your custom matrix evolution
<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/21aa0f0c-7dc1-4355-bb45-6ba11bb246ba" />
<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/ca18565d-1323-492e-88b7-44a91e8d49ab" />

# Adjust the possible dimensions of the matrix
The program assumes a square lattice, but if this is not the case you can adjust to the desired dimensions
<img width="700" height="700" alt="image" src="https://github.com/user-attachments/assets/c771e978-b0e8-40b1-a77a-db6f20dc4378" />

# Add custom color palettes
Create your own colors and map the values to beatify the lattice

<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/918e5f3c-965d-4091-80df-1655efe79bd1" />
<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/c4581e49-2a91-446b-8826-eb9fa627d662" />

## Requirements
You'll need g++ compilator and the raylib libraries:

# g++
```
sudo apt update
sudo apt install g++
```

# raylib
```
sudo apt install libraylib-dev
```

# base libraries
```
sudo apt install libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxcursor-dev libxinerama-dev libxxf86vm-dev
```

# make 
```
sudo apt install make
```



# Instalation
First you have to dowload the project and compile

```
git clone https://github.com/luccaPossamai/matrixes.git
cd matrixes
make
```
```
sudo mv matrix /usr/local/bin/matrix
```
