

#include "Unique.h"

void findUnique::UpdateDeltaMatrix(vector<String> & haplotypes, vector<int> & index,
          vector<int> & firstDifference, int length, int blockSize,
          vector<int> & oldIndex,  vector<int> & previousPredecessor,  vector<int> & previousDifference)
{
    int haps = index.size();
    previousPredecessor[oldIndex[0]] = -1;
    for (int i = 1; i < haps; i++)
    {
        previousPredecessor[oldIndex[i]] = oldIndex[i-1];
        previousDifference[oldIndex[i]]  = firstDifference[i-1];
    }

    for (int i = 1; i < haps; i++)
    {

        if (index[i-1] == previousPredecessor[index[i]])
        {
            firstDifference[i-1] =
            haplotypes[index[i]][length - 1] ==
            haplotypes[index[i-1]][length - 1] ?
            previousDifference[index[i]] + 1 : 0;
            continue;
        }

        int diff = 0;
        while (diff < length && diff < blockSize &&
               haplotypes[index[i]][length - diff - 1] ==
               haplotypes[index[i-1]][length - diff - 1])
        diff++;

        firstDifference[i - 1] = diff;
    }
}


void findUnique::AnalyzeBlocks(
         vector<int> & index, vector<int> & firstDifference, int length, int blockSize,
         vector<int> & cost,
         vector<int> & bestSlice, vector<int> & bestComplexity, vector<vector<int> > &bestIndex)
{
    int haps = index.size();

    // Try to figure out optimal block split
    for (int i = 1; i <= blockSize && i <= length; i++)
    {
        int distinctHaplos = 1;

        for (int j = 0; j < haps - 1; j++)
            if (i > firstDifference[j])
                distinctHaplos++;

      int currentCost=1;

      if(i>1)
        currentCost= transFactor * haps + (i) * distinctHaplos  * cisFactor + cost[length - i+1];
        if (i==2)
         {
         cost[length] = currentCost;
         bestSlice[length] = 2;
         bestComplexity[length] = distinctHaplos;
         }
      else if (cost[length] > currentCost)
         {
         cost[length] = currentCost;
         bestSlice[length] = i;
         bestComplexity[length] = distinctHaplos;
         }
      else if (cost[length] + transFactor * haps < currentCost)
         break;
      }
    bestIndex[length-1] = index;

   }




double findUnique::FlushBlocks(vector<ReducedHaplotypeInfo> &HapInfo,
                               vector<variant> &varList, int LastflushPos,vector<String> & haplotypes, vector<int> &MaxAllele,
                                vector<int> & cost,
                   vector<int> & bestComplexity, vector<int> & bestSlice, vector<vector<int> > &bestIndex)
{

    ReducedHaplotypeInfo tempInfo;
    int where   = haplotypes[0].Length();
    where--;
    int newCost = cost[where+1];
    int totalBlocks=0;
    double totalComplexity = 0;
    vector<int> examplars, labels;

    vector<int> blockBoundaries;
    blockBoundaries.clear();

    // Work out optimal set of block boundaries

    while (where != 0)
      {
      blockBoundaries.push_back(where);
    totalBlocks++;
      totalComplexity += bestComplexity[where+1];
    where = where - bestSlice[where+1]+1;
 };



    int blockStart = 0;
//
    while (blockStart != (haplotypes[0].Length()-1))
    {

        int blockEnd = blockBoundaries.back();
        blockBoundaries.pop_back();
        examplars.clear();
        examplars.push_back(bestIndex[blockEnd][0]);

        labels.resize(bestIndex[blockEnd].size());
        labels[bestIndex[blockEnd][0]] = 0;
        int countCard=0;

        for (int i = 1; i < (int) bestIndex[blockEnd].size(); i++)
        {
            int previous = bestIndex[blockEnd][i-1];
            int current  = bestIndex[blockEnd][i];
            countCard++;
            for (int j = blockStart; j <= blockEnd; j++)
            if (haplotypes[previous][j] != haplotypes[current][j])
               {
                examplars.push_back(current);
               break;
               }

         labels[current] = examplars.size() - 1;

         }

        tempInfo.uniqueIndexMap=labels;

        tempInfo.startIndex=blockStart+LastflushPos;
        tempInfo.endIndex=LastflushPos+blockEnd;
        tempInfo.uniqueCardinality.clear();
        tempInfo.uniqueCardinality.resize(examplars.size(),0);
        tempInfo.InvuniqueCardinality.clear();
        tempInfo.InvuniqueCardinality.resize(examplars.size(),0.0);




        for (int i = 0; i < (int)labels.size(); i++)
        {
            tempInfo.uniqueCardinality[labels[i]]++;
        }
         for (int i = 0; i < (int)examplars.size(); i++)
        {
            tempInfo.InvuniqueCardinality[i]=1.0/(float)tempInfo.uniqueCardinality[i];
        }




        tempInfo.uniqueHaps.resize(examplars.size());
        tempInfo.uniqueIntHaps.resize(examplars.size());
        for (int i = 0; i < (int)examplars.size(); i++)
            {
                tempInfo.uniqueHaps[i].resize(blockEnd-blockStart+1);
                tempInfo.uniqueIntHaps[i].resize(blockEnd-blockStart+1);
            }

        for (int j = 0; j < (int)examplars.size(); j++)
       {
// cout<<j <<" = "<<haplotypes[examplars[j]]<<endl;

           for (int i = blockStart; i <= blockEnd; i++)
            {
                tempInfo.uniqueHaps[j][i-blockStart]=haplotypes[examplars[j]][i]=='0'?false:true;


//                cout<<" UMMAA = "<<tempInfo.uniqueHaps[j][i-blockStart]<<endl;

//                int tempVal =(int)(haplotypes[examplars[j]][i]-'0')-1;


//                tempInfo.uniqueIntHaps[j][i-blockStart]= tempVal;
//cout<<haplotypes[examplars[j]][i]<<"\t"<< tempInfo.uniqueIntHaps[j][i-blockStart]<<endl;
//                uniqueIntHaps
            }

    }

        HapInfo.push_back(tempInfo);

         blockStart = blockEnd;

    }

    return newCost;
    }





//
//double findUnique::FlushBlocks(vector<int> &optEndPoints,vector<ReducedHaplotypeInfo> &HapInfo,
//                               vector<variant> &varList, int LastflushPos,vector<String> & haplotypes,
//                                vector<int> & cost,
//                   vector<int> & bestComplexity, vector<int> & bestSlice, vector<vector<int> > &bestIndex)
//{
//
//    ReducedHaplotypeInfo tempInfo;
//    int where   = haplotypes[0].Length();
//    where--;
//    int newCost = cost[where+1];
//    int totalBlocks=0;
//    double totalComplexity = 0;
//    vector<int> examplars, labels;
//
//    vector<int> blockBoundaries;
//    blockBoundaries.clear();
//
//    // Work out optimal set of block boundaries
//
//    while (where != 0)
//      {
//      blockBoundaries.push_back(where);
//    totalBlocks++;
//      totalComplexity += bestComplexity[where+1];
//    where = where - bestSlice[where+1]+1;
// };
//
//
//
//    int blockStart = 0;
////
//    while (blockStart != (haplotypes[0].Length()-1))
//    {
//
//        int blockEnd = blockBoundaries.back();
//        blockBoundaries.pop_back();
//        examplars.clear();
//        examplars.push_back(bestIndex[blockEnd][0]);
//
//        labels.resize(bestIndex[blockEnd].size());
//        labels[bestIndex[blockEnd][0]] = 0;
//        int countCard=0;
//
//        for (int i = 1; i < (int) bestIndex[blockEnd].size(); i++)
//        {
//            int previous = bestIndex[blockEnd][i-1];
//            int current  = bestIndex[blockEnd][i];
//            countCard++;
//            for (int j = blockStart; j <= blockEnd; j++)
//            if (haplotypes[previous][j] != haplotypes[current][j])
//               {
//                examplars.push_back(current);
//               break;
//               }
//
//         labels[current] = examplars.size() - 1;
//
//         }
//
//        tempInfo.uniqueIndexMap=labels;
//
//        tempInfo.startIndex=blockStart+LastflushPos;
//        tempInfo.endIndex=LastflushPos+blockEnd;
//        tempInfo.uniqueCardinality.clear();
//        tempInfo.uniqueCardinality.resize(examplars.size(),0);
//        for (int i = 0; i < (int)labels.size(); i++)
//        {
//            tempInfo.uniqueCardinality[labels[i]]++;
//
//        }
//
//
//
//        tempInfo.uniqueHaps.resize(examplars.size());
//        for (int i = 0; i < (int)examplars.size(); i++)
//            tempInfo.uniqueHaps[i].resize(blockEnd-blockStart+1);
//
//        for (int j = 0; j < (int)examplars.size(); j++)
//       {
//
//
//           for (int i = blockStart; i <= blockEnd; i++)
//            {
//                tempInfo.uniqueHaps[j][i-blockStart]=haplotypes[examplars[j]][i]=='0'?varList[i+LastflushPos].refAllele:varList[i+LastflushPos].altAllele;
//
//                cout<<" UMM = "<<tempInfo.uniqueHaps[j][i-blockStart]<<endl;
//            }
//
//    }
//
//        HapInfo.push_back(tempInfo);
//
//         blockStart = blockEnd;
//
//    }
//
//    return newCost;
//    }
//
//

