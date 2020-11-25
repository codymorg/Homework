import math as math #sqrt
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.naive_bayes import GaussianNB

## Settings ##
DATA_LOCATION = "Iris.csv" 
DEPENDENT = "Species"  
SKIP_PLOT_GENERATION = True

IGNORE = ["Id", DEPENDENT]

TRAINING_PERCENT = 0.8   # These should sum to 1.0
TESTING_PERCENT = 0.2    # These should sum to 1.0
if abs(TRAINING_PERCENT + TESTING_PERCENT - 1.0) > 0.01:
    raise Exception("Bad data splitting values")
    

### Global Variables ###
labels_g = None      # list of data labels 
data_g = {}          # dictionary of data as [attribute][list of data] 
range_g = {}         # dictionary of range for each attribute as [attribute][min,max]
standard_g = {}      # standardized data
correlation_g = None # correlation matrix for heat map
training_g = {}      # training section of data [default 80%]
testing_g = {}       # testing section [default 10%]
datapointCount_g = 0 # how many data points are in the data?
dependentValues = {} # convert dependent values to integers - keet track of that conversion

dependentProbability_g = {} # how likily is each dependent 

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
def CalcDeviation(key,value):
    count = len(data_g[key])
    mean = (range_g[key][0] + range_g[key][1]) / 2 
    return math.sqrt(value / count)

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
def GetMean(attribute, result):
    sum = 0.0
    i = 0
    total = 0
    for value in testing_g[attribute]:
        total+=1
        if result == testing_g[DEPENDENT][i]:
            sum+=value
        i+=1
    return sum / total
def Evaluate(datapoint):
    gauss ={}
    results={}
    for result in dependentValues:
        for key in testing_g:
            if key not in IGNORE:
                mean = GetMean(key, dependentValues[result])
                x = datapoint[key]
                stdev = CalcDeviation(key, x)
                exponent = math.exp(-0.5 * ((x - mean) / stdev)**2)
                gauss[key] =(1 / (math.sqrt(2 * math.pi) * stdev)) * exponent
        for key in gauss:
            if dependentValues[result] not in results:
                results[dependentValues[result]] = 1
            value = gauss[key]
            if value != 0:
                results[dependentValues[result]] *= value
    highest = 0
    highestKey = 0
    for key in results:
        if results[key] > highest:
            highest = results[key]
            highestKey = key

    return highestKey

def GetDepName(value):
    for key in dependentValues:
        if dependentValues[key] == value:
            return key
    


def GetPortionOfData(percent, start):
    portion = {}
    start = int(start)
    end = int(percent * datapointCount_g) + start

    for key in data_g:
        i = 0
        portion[key] = []
        for value in data_g[key]:
            if i >= start:
                portion[key].append(value)
            i+=1
            if i >= end:
                break

    return portion #split data into training, validation, testing

def BuildDataPoint(dataset, i):
    test = {}
    for key in dataset:
        test[key] = dataset[key][i]
    return test #build a datapoint[i] 

def PrintAccuracy(isReal):
    testingLen = len(testing_g[DEPENDENT])
    if(not isReal):
        for key in standard_g:
            for i in range(0,len(testing_g)):
                testing_g[key][i] = standard_g[key][i]
    accuracy = 0.0
    correct = 0

    for i in range(0, testingLen):
        datapoint = BuildDataPoint(testing_g, i)

        # Built in library accuracy
        evalValue = 0
        if isReal == True:
            list = []
            for key in datapoint:
                if key not in IGNORE:
                    list.append(datapoint[key])
            evalValue = model.predict([list])[0]
        else:
            evalValue = Evaluate(datapoint)
        correctValue = testing_g[DEPENDENT][i]
        if evalValue == correctValue : correct+=1
    
    accuracy = correct / testingLen
    if(isReal) : print("Evaluating Built in library:")
    else       : print("Evaluating hand build functions:")

    print("Correct:",correct," total:",testingLen)
    print("Accuracy:",round(100*accuracy,2), "% on Testing set\n") #print accuracy data 
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
                if tokList[i] not in dependentValues:
                    dependentValues[tokList[i]] = len(dependentValues)
                data_g[label].append(dependentValues[tokList[i]])
                range_g[label] = (Min(dependentValues[tokList[i]], range_g[label][0]), Max(dependentValues[tokList[i]], range_g[label][1]))
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
    if key not in IGNORE:
        standard_g[key] = []
        for value in data_g[key]:
            standard_g[key].append(Standardize(key, value))
print("All values standardized [0,1]")

### Plot Heatmaps ###
StandardDeviation()
if SKIP_PLOT_GENERATION == False:
    print("Creating heatPlot...\n")
    #correlation_g = BuildCorrelation()
    plt.figure(figsize=(16,16))
    df = pd.read_csv(DATA_LOCATION)
    corr = df.corr()
    heatmap = sns.heatmap(corr, vmin=-1, vmax=1, center=0, cmap="coolwarm")
    figure = heatmap.get_figure()
    figure.savefig("heat.png")
    figure.clf()
else:
    print("Plot generation omitted")

### split data ###
training_g = GetPortionOfData(TRAINING_PERCENT,0)
testing_g = GetPortionOfData(TRAINING_PERCENT,(TRAINING_PERCENT) * datapointCount_g)

### prior probabilities P(Y)###
for value in data_g[DEPENDENT]:
    if value not in dependentProbability_g:
        dependentProbability_g[value] = 0
    dependentProbability_g[value]+=1
for key in dependentProbability_g:
    dependentProbability_g[key]/=datapointCount_g


### Real Evaluation ###
model = GaussianNB()
features =[]
index = int(TRAINING_PERCENT * datapointCount_g)
for i in range(0,index ):
    list = []
    for key in data_g:
        if key not in IGNORE:
            list.append(data_g[key][i])
    features.append(tuple(list))
label = data_g[DEPENDENT][:index]
model.fit(features,label)
PrintAccuracy(True)

PrintAccuracy(False)

### Evaluate ###
inp = None
while(inp != "n"):
    print("Do you want to enter data to make a prediction? [Y]es No")
    inp = input()
    inp = inp.upper()
    if inp == "N":
        break

    string = "\n"
    for label in labels_g:
        if label not in IGNORE:
            string += label + "  "
    print(string)
    print("enter a comma seperated continuous list of data")
    toks = input().split("\n")[0].split(",")
    if(len(toks) == len(labels_g)-len(IGNORE)):
        datapoint = {}
        i = 0
        for label in labels_g:
            if label not in IGNORE:
                denom = range_g[label][1] - range_g[label][0]
                try:
                    datapoint[label] = (float(toks[i]) - range_g[label][0]) / denom
                except:
                    datapoint[label] = (ord(toks[i]) - range_g[label][0]) / denom
                i+=1
        print("Result: ", GetDepName(Evaluate(datapoint)))
    else:
        print("Mismatching data point, you are probably missing a column or a comma")



