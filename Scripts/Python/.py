import os
def encrypt()
   with open('zabvenie.exe', 'rb') as f
       arr = f.read()
       with open('encrypted.txt', 'w') as w
           arr2 = []
           for i in arr
               if i % 2
                   arr2.append(int(i) - 25) # Абсолютно любая цифра
               else
                   arr2.append(int(i) + 15) # Абсолютно любая цифра
           for i in arr2
               w.write(str(i) +  )
encrypt()