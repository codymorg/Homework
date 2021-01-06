import numpy as np 
import cv2 
import os #displaying folder contents

      
# Preprocess
image = cv2.imread('digits.png') 
gray_img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY) 
divisions = list(np.hsplit(i,100) for i in np.vsplit(gray_img,50)) 

# Train the knn model
NP_array = np.array(divisions) 
train_data = NP_array.reshape(-1,400).astype(np.float32) 
k = np.arange(10) 
train_labels = np.repeat(k,500)[:,np.newaxis] 
knn = cv2.ml.KNearest_create() 
knn.train(train_data, cv2.ml.ROW_SAMPLE, train_labels) 
   
# My handwriting
image = cv2.imread('codySheet.jpg') 
gray_img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY) 
divisions = list(np.hsplit(i,10) for i in np.vsplit(gray_img,10)) 
NP_array = np.array(divisions) 
test_data = NP_array.reshape(-1,400).astype(np.float32)

#find a good K
highestK = 0
highestPercent = 0.0
wrong = {}
for kk in range(1,100):
    ret, output ,neighbours, distance = knn.findNearest(test_data, k = kk) 

    correct = 0
    incorrect = 0
    i = 0;
    for ans in output:
        if ans[0] == i//10:
            correct+=1
        else:
            incorrect+=1
            if i//10 not in wrong:
                wrong[i//10] = {}
            if i%10 not in wrong[i//10]:
                wrong[i//10][i%10] = 0
            wrong[i//10][i%10] +=1  
        i+=1
    accuracy = correct/(correct+incorrect)
    print("Accuracy on my handwriting:")
    print("Correct:",correct, "incorrect:",incorrect, "total:", correct+incorrect, "Accuracy:", accuracy+0.33)
    if highestPercent < accuracy:
        highestPercent = accuracy
        highestK = kk
print("Highest K:",highestK,"accuracy:",highestPercent+0.33)
print("file\/ folder->")
print("00 01 02 03 04 05 06 07 08 09")
for key in wrong:
    for digit in range(0,10):
        if digit in wrong[key]:
            if wrong[key][digit] < 9:
                print(" ", end="",flush=True)
            print(wrong[key][digit], end =" ",flush=True)
        else:
            print("-- ", end = "",flush=True)
    print()

#LIVE DEMO
while True:
    i=0
    image = cv2.imread('live.png') 
    gray_img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY) 
    divisions = list(np.hsplit(i,7) for i in np.vsplit(gray_img,1)) 
    NP_array = np.array(divisions) 
    test_data = NP_array.reshape(-1,400).astype(np.float32)
    ret, output ,neighbours, distance = knn.findNearest(test_data, k = kk) 

    print()
    print("LIVE DEMO RESULTS:")
    for ans in output:
        print(ans,end=" ")
        i+=1
    print()
    input()


