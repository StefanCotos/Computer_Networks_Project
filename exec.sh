cd Cerinte_prob

g++ $1.cpp -o $1
if [ $? -eq 0 ]
then
    echo "y" 
    ./$1
else
    echo "n"         
fi