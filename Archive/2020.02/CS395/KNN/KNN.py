import math as math #sqrt
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

## Settings ##
DATA_LOCATION = "EvaluationData.csv" 
DEPENDENT = "CPI"             
SKIP_PLOT_GENERATION = True         

IGNORE = ["COUNTRY ID", DEPENDENT]

TRAINING_PERCENT = 1.0   # These should sum to 1.0
VALIDATION_PERCENT = 0.0 # These should sum to 1.0
TESTING_PERCENT = 0.0    # These should sum to 1.0
if abs(TRAINING_PERCENT + VALIDATION_PERCENT + TESTING_PERCENT - 1.0) > 0.01:
    raise Exception("Bad data splitting values")
    

### Global Variables ###
labels_g = None      # list of data labels 
data_g = {}          # dictionary of data as [attribute][list of data] 
range_g = {}         # dictionary of range for each attribute as [attribute][min,max]
standard_g = {}      # standardized data
correlation_g = None # correlation matrix for heat map
training_g = {}      # training section of data [default 80%]
validation_g = {}    # validation section [default 20%]
testing_g = {}       # testing section [default 10%]
datapointCount_g = 0 # how many data points are in the data?
k_g = 3


### Functions ###
def Min(a, b):
    if a is None:
        return b
    if b is None:
        return a
    return min(a,b) #treat None as a special case - not absolute minimum
def Max(a, b):
    if a is None:
        return b
    if b is None:
        return a
    return max(a,b) 
def Clamp(value, mn, mx):
    if value >= mx:
        return mx
    if value <= mn:
        return min
    
    return value #value is now in range of[min, max]

def Standardize(name, value):
    mn = range_g[name][0]
    mx = range_g[name][1]

    #standardize a given value for a given name
    return (value - mn) / (mx - mn) #standardize all data [0,1]   
def StandardDeviation():
    deviation = {} #[name][standard deviation]
    for key in data_g:
        count = len(data_g[key])
        sum = 0.0
        mean = (range_g[key][0] + range_g[key][1]) / 2 
        for value in data_g[key]:
            sum += (value - mean) ** 2
        deviation[key] = math.sqrt(sum / count)
    return deviation #internal - build standard deviation for heatmap
def BuildCorrelation():
    zValues = {}
    deviation = StandardDeviation()
    correlation_g = {}
    
    # generate z values
    for key in data_g:
        zValues[key] = []
        mean = (range_g[key][0] + range_g[key][1]) / 2
        for value in data_g[key]:
            zValues[key].append((value - mean) / deviation[key])

    # generate r values
    rValues = {}
    count = 0
    for key in zValues:
        sum = 0
        i = 0
        for value in zValues[key]:
            sum += value * zValues[DEPENDENT][i]
            i += 1 
        count = len(data_g[key])
        rValues[key] = Clamp(sum / (count - 1), -1.0, 1.0)

    for keyB in zValues:
        correlation_g[keyB] = {}
        for keyA in zValues:
            sum = 0
            i = 0
            for value in zValues[keyA]:
                sum += value * zValues[keyB][i]
                i += 1
            correlation_g[keyB][keyA] = Clamp(sum / (count - 1), -1.0, 1.0)  #internal - build correlation mat for heatmap
      
def GetPortionOfData(percent, start):
    portion = {}
    start = int(start)
    end = int(percent * datapointCount_g) + start

    for key in standard_g:
        i = 0
        portion[key] = []
        for value in standard_g[key]:
            if i >= start:
                portion[key].append(value)
            i+=1
            if i >= end:
                break

    return portion #split data into training, validation, testing

def WeightedDistances(distances):
    weighted = []
    for d in distances:
        weighted.append(1/d)
    return weighted      #give the weighted distance evaluation
def Distance(data):
    trainingCount = len(training_g[DEPENDENT])
    distances = []

    # calc distance
    for i in range(0, trainingCount):
        sum = 0
        for key in data:
            if key not in IGNORE:
                trainValue = training_g[key][i]
                evalValue = data[key]
                sum += (trainValue - evalValue) ** 2
            else:
                continue
        depVar = training_g[DEPENDENT][i]
        distance = sum#math.sqrt(sum)
        distances.append((depVar, distance))

    for d in distances:
        print(d[1])
    input()
    distances.sort(key = lambda x:x[1])

    return distances                    #evaluate the distance bewtten datapoint and trainingdata set
def Evaluate(data, k, distances=None):
    sum = 0
    if distances == None:
        distances = Distance(data)

    for i in range(0,k):
        sum += distances[i][0]

    return int(round(sum / k)) #evalute knn for a given k value, distances may be provided if it turns out to be too time consuming

def BuildDataPoint(dataset, i):
    test = {}
    for key in dataset:
        test[key] = dataset[key][i]
    return test #build a datapoint[i] 

def PrintAccuracy():
    return
    testingLen = len(testing_g[DEPENDENT])
    accuracy = 0.0
    correctY = 0
    correctN = 0
    totalY = 0
    totalN = 0

    for i in range(0, testingLen):
        datapoint = BuildDataPoint(testing_g, i)

        evalValue = Evaluate(datapoint, k_g)
        correctValue = testing_g[DEPENDENT][i]

        if evalValue == 1.0 and correctValue == 1.0: correctY+=1
        if evalValue == 0.0 and correctValue == 0.0: correctN+=1
        if correctValue == 1.0 : totalY+=1
        if correctValue == 0.0 : totalN+=1
        if evalValue == correctValue : accuracy+=1
    
    accuracy /= testingLen
    print("Yes Stats: ", correctY," / ",totalY)
    print("No Stats: ", correctN," / ",totalN)
    print("k=", k_g, " has an accuracy of ", round(100*accuracy,2), "% on Testing set\n") #print accuracy data 

    return accuracy

### Load the data ###
print("Loading data...\n")
dataFile = open(DATA_LOCATION, "r")
rawData = dataFile.readlines()
for row in rawData:
    tokList = row.split("\n")[0].split(",")
    if labels_g is None:
        labels_g = tokList
    else:
        i = 0
        for label in labels_g:
            if label not in data_g:
                data_g[label] = []
                range_g[label] = (None, None) #(Min, Max)
            try:
                asf = float(tokList[i])
                data_g[label].append(asf)
                range_g[label] = (Min(asf, range_g[label][0]), Max(asf, range_g[label][1]))
            except:
                asi = ord(tokList[i])
                data_g[label].append(asi)
                range_g[label] = (Min(asi, range_g[label][0]), Max(asi, range_g[label][1]))
            i = i+1


### Check for missing values ###
for key in data_g:
    length = len(data_g[key])
    if datapointCount_g != 0 and datapointCount_g != length:
        print("ERROR!\ns" + key + " has " + str(length) + ' values! Everyone else has ' + str(datapointCount_g))
        raise Exception("Missing Value")
    datapointCount_g = length
print("Loading file complete - no missing values")

### Standardize data ###
for key in data_g:
    standard_g[key] = []
    for value in data_g[key]:
        standard_g[key].append(Standardize(key, value))
print("All values standardized [0,1]")

### Plot Heatmaps ###
if SKIP_PLOT_GENERATION == False:
    print("Creating heatPlot...\n")
    correlation_g = BuildCorrelation()
    plt.figure(figsize=(16,16))
    df = pd.read_csv(DATA_LOCATION)
    corr = df.corr()
    heatmap = sns.heatmap(corr, vmin=-1, vmax=1, center=0, cmap="coolwarm")
    figure = heatmap.get_figure()
    figure.savefig("heat.png")
    figure.clf()

    ### Plot Scatter matrix ###
    i = 0.0
    for key in standard_g:
        print("creating plot for " + key + " " + str(round((100*i) / len(labels_g), 2)) + "% ...\n")
        i+=1
        plt.scatter(standard_g[key], standard_g[DEPENDENT])
        plt.xlabel(key)
        plt.ylabel(DEPENDENT)
        plt.savefig("Plots/"+key+"_Plot.png")
        plt.clf()
    print("Plot generation Complete")
else:
    print("Plot generation omitted")

#Ask for any other correlation matrices
inp = None
while(inp != "n"):
    print("would you like to see any correlation maxtrices? Yes [N]o")
    inp = input()
    inp = inp.upper()
    if inp != 'Y':
        break
    
    labelStr = ""
    for label in labels_g:
        labelStr += label + " "
    print(labelStr)
    print("what feature would you like to compare?")
    corrA = input()
    print()
    print("what other feature would you like to compare?")
    corrB = input()
    print()
    if corrA in standard_g and corrB in standard_g:
        plt.scatter(standard_g[corrA], standard_g[corrB])
        plt.xlabel(corrA)
        plt.ylabel(corrB)
        plt.show()
        plt.clf()
    else:
        print("either " + corrA + " or " +corrB + " do not exist")


### split data ###
training_g = GetPortionOfData(TRAINING_PERCENT,0)
validation_g = GetPortionOfData(VALIDATION_PERCENT, TRAINING_PERCENT * datapointCount_g)
testing_g = GetPortionOfData(TRAINING_PERCENT,(TRAINING_PERCENT + VALIDATION_PERCENT) * datapointCount_g)


### calculate K ###
kAccuracies = []
kValues = []
print("Do you want to calculate K? Yes [N]o  default k =", k_g)
inp = input()
inp = inp.upper()
if inp == "Y":
    print("Calculating k...\n")
    accuracyMax = 0.0
    validLen = len(validation_g[DEPENDENT])
    end = int(math.sqrt(datapointCount_g))
    for possibleK in range(1, end):
        print("testing k=", possibleK, "...\n")
        accuracy = 0.0

        for i in range(0,validLen):
            test = {}
            for key in validation_g:
                test[key] = validation_g[key][i]
            evalValue = Evaluate(test, possibleK)
            correctValue = validation_g[DEPENDENT][i]
            if evalValue == correctValue: 
                accuracy+=1
    
        accuracy /= validLen
        kAccuracies.append(accuracy)
        kValues.append(possibleK)

        if accuracy > accuracyMax:
            print (round(accuracy,4), " is better than ", round(accuracyMax,4), "k=", possibleK)
            accuracyMax = accuracy
            k_g = possibleK

    print("k=", k_g, " has an accuracy of ", round(100*accuracyMax,2), "% on Validation set\n")

    ### Plot Accuracies ###
    plt.scatter(kValues, kAccuracies)
    plt.xlabel("K-Values")
    plt.ylabel("Accuracy")
    plt.show()
    plt.clf()
else:
    print("Skipping K generation k = ",k_g)


### calcualte accuracy ###
print("Calculating accuracy on testing set...\n")
originalAccuracy = PrintAccuracy()


### Test for different ignored attributes
print("Attempting to improve accuracy by ignoreing various labels...\n")
originalIgnore = IGNORE.copy()
bestAccuracy = originalAccuracy
#
#for label in labels_g:
#   if label not in IGNORE:
#       print("Testing accuracy if ", label, " is ignored...")
#       IGNORE.append(label)
#       acc = PrintAccuracy()
#
#       if(acc > bestAccuracy):
#           bestAccuracy = acc
#           print("Adding to ignorelist: ",label)
#           print(IGNORE,"\n")
#       else:
#           IGNORE.remove(label)
#           print(IGNORE)
#            
print(originalIgnore)
print("Accuracy with original Ignore list: ", originalAccuracy,"\n")
print(IGNORE,"\n")
string = ""
for label in labels_g:
    if label not in IGNORE:
        string += label + ", "

print("Keeping: "+string+"\n")
print("Accuracy with new ignore list", bestAccuracy,"\n")
print("Keep new Ignore list? [Y]es No?")
inp = input()
inp = inp.upper()
if(inp == "N"):
    IGNORE = originalIgnore
print("Using ignore list:", IGNORE,"\n")


### Evaluate ###
inp = None
while(inp != "n"):
    print("Do you want to enter data to make a prediction? Yes [N]o")
    inp = input()
    inp = inp.upper()
    if inp != "Y":
        break

    print("enter a comma seperated continuous list of data")
    toks = input().split("\n")[0].split(",")
    if(len(toks) == len(labels_g)):
        datapoint = {}
        i = 0
        for label in labels_g:
            denom = range_g[label][1] - range_g[label][0]
            try:
                datapoint[label] = (float(toks[i]) - range_g[label][0]) / denom
            except:
                datapoint[label] = (ord(toks[i]) - range_g[label][0]) / denom
            i+=1
        print("Result: ", Evaluate(datapoint, k_g))
    else:
        print("Mismatching data point, you are probably missing a column or a comma")