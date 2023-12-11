#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<fstream>
#include<sstream>
#include<cctype>
#include<algorithm>
#include <math.h>
using namespace std;
string  stringtolower(string word)
{
    for (int i = 0; i < word.size(); i++)
    {
        word[i] = tolower(word[i]);
    }
    return word;
}
struct  postinglist
{
    string  file;
    int wordCount;
    int Qindex;
    int wordFileCount;
    float docWei;
};
class   invertedIndex
{
    map<string, vector<postinglist> > dictionary;
    vector<string> fileList;
    map<string, vector<postinglist> > fileWords;
    vector<vector<string>> retrievalResult;
    map<string, vector<postinglist> > tfDic;
    map<string, map<string, vector<postinglist> > > wordFileCount; //tfidf query
    map<string, vector<postinglist>> docWeight;
    map<string, vector <postinglist>> wordallFileCount; //allfiles
    map<string, vector<int>> everyfileWords;//how much of a word is in a Doc
public:
    void showFiles();
    void addFile(string filename);
    void retrieval(string word);
    void finalResult();
    void tfidf(vector <string> Qwords);

};
void    invertedIndex::showFiles()
{
    int size = fileList.size();
    for (int i = 0; i < size; i++)
        cout << i + 1 << ": " << fileList[i] << endl;

    if (!size) cout << "No files found\n";
}
void    invertedIndex::addFile(string filename)
{
    ifstream fp;
    fp.open(filename, ios::in);

    if (!fp)
    {
        cout << "File Not Found!\n";
        return;
    }

    for (int i = 0; i < fileList.size(); i++)
        if (fileList[i] == filename)
        {
            cout << "File Added\n";
            return;
        }

    cout << "File Added\n";
    fileList.push_back(filename);
    string line, word;
    map<string,vector<string>> allfileWords;
    
    while (getline(fp, line))
    {
        stringstream s(line);
        while (s >> word)
        {
            word = stringtolower(word);
            postinglist obj;
            obj.file = filename;
            dictionary[word].push_back(obj);
            allfileWords[filename].push_back(word);
        }
    }
    fp.close();
    map<string, vector <string>> uniqueallfileWords;
    
    uniqueallfileWords[filename] = allfileWords[filename];
    sort(uniqueallfileWords[filename].begin(), uniqueallfileWords[filename].end());
    uniqueallfileWords[filename].erase(unique(uniqueallfileWords[filename].begin(), uniqueallfileWords[filename].end()), uniqueallfileWords[filename].end());
    postinglist obj;
    //how much are all words in a doc
    obj.wordCount = allfileWords[filename].size();    
    fileWords[filename].push_back(obj);
    
    //document weight for tfidf
    for (int i = 0; i < uniqueallfileWords[filename].size(); i++)
    {
        //test cout << "word: " << uniqueallfileWords[filename][i] << endl;
        int counter = 0;
        for (int j = 0; j < allfileWords[filename].size(); j++)
        {
            if (uniqueallfileWords[filename][i] == allfileWords[filename][j])
            {
                counter++;
            }
        }
        //test cout <<" num of rep: "<< counter << endl;
        everyfileWords[uniqueallfileWords[filename][i]].push_back(counter);
    }
    float weight = 0;
    for (int i = 0; i < uniqueallfileWords[filename].size(); i++)
    {    
        weight += pow(1 + log10(everyfileWords[uniqueallfileWords[filename][i]][0]), 2);
        //test cout <<"wei: " <<sqrt (weight) << endl;
    }
    postinglist obj2;
    obj2.docWei = sqrt(weight);
    docWeight[filename].push_back(obj2);
    everyfileWords.clear();
}
void    invertedIndex::retrieval(string word)
{
    vector <string> wordFiles;

    if (dictionary.find(word) == dictionary.end())
    {
        cout << "No instance exist\n";
        return;
    }

    for (int i = 0; i < dictionary[word].size(); i++)
        wordFiles.push_back(dictionary[word][i].file);

    cout << dictionary[word].size() << " Times: " << word << "\t->" << " File Name:\t";

    sort(wordFiles.begin(), wordFiles.end());
    wordFiles.erase(unique(wordFiles.begin(), wordFiles.end()), wordFiles.end());

    for (int i = 0; i < wordFiles.size(); i++)
        cout << wordFiles[i] << "\t";
    cout << endl;

    retrievalResult.push_back(wordFiles);
}
void    invertedIndex::finalResult()
{
    vector <string> searchResult;
    vector <string> t;
    int i = 0, j = 0, k = 0;
    if (retrievalResult.size() == 0)
    {
        return;
    }
    //Just Extract the First Column Data of retrievalResult In t 
    for (int i = 0; i < retrievalResult[0].size(); i++)
    {
        t.push_back(retrievalResult[0][i]);
    }
    // 0 1 5 
    // 0 1 2 3 4 5 6    Moved to t                     (retrievalResult 0)
    // 0 1 2 3 4 5 6    Still is in retrievalResult    (retrievalResult 1)
    // 0 1 2 3 4 5 6
    for (k = 1; k < retrievalResult.size(); k++)
    {
        for (i = 0; i < t.size(); i++)
        {
            for (j = 0; j < retrievalResult[k].size(); j++)
            {
                if (t[i] == retrievalResult[k][j])
                {
                    searchResult.push_back(t[i]);
                }
            }
        }
        if (searchResult.size() == 0)
        {
            cout << "Nothing found !!!" << endl << endl;
            return;
        }
        t.clear();
        for (int l = 0; l < searchResult.size(); l++)
        {
            t.push_back(searchResult[l]);
        }
        searchResult.clear();
    }

    if (t.size() != 0)
    {
        cout << "Final Result: ";
        for (int i = 0; i < t.size(); i++)
            cout << t[i] << "\t";
    }

    searchResult.clear();
    retrievalResult.clear();
    cout << endl << endl;
}

void    invertedIndex::tfidf(vector<string> Qwords)
{
    
    if (fileList.size() == 0)
    {
        cout << "No File Available\n";
        return;
    }
    
    tfDic.clear();
    
    for (int i = 0; i < Qwords.size(); i++)
    {
        postinglist obj;
        obj.Qindex = i;
        tfDic[Qwords[i]].push_back(obj);
    }

    sort(Qwords.begin(), Qwords.end());
    Qwords.erase(unique(Qwords.begin(), Qwords.end()), Qwords.end());

    vector <string> uniqueFiles;
    vector <vector<string>> vec;
    for (int i = 0; i < Qwords.size(); i++)
    {
        uniqueFiles.clear();
        // term1 doc1 doc1 doc1 doc2 doc2 doc2
        for (int j = 0; j < dictionary[Qwords[i]].size(); j++)
            uniqueFiles.push_back(dictionary[Qwords[i]][j].file);
        //term1 doc1 doc2
        sort(uniqueFiles.begin(), uniqueFiles.end());
        uniqueFiles.erase(unique(uniqueFiles.begin(), uniqueFiles.end()), uniqueFiles.end());
        //vec0 <- term 1 doc1 doc2
        vec.push_back(uniqueFiles);

    }
    //test vec
    //for (int i = 0; i < vec.size(); i++)
    //{
    //    for (int j = 0; j < vec[i].size(); j++)
    //    {
    //        cout << vec[i][j] << "\t";
    //    } 
    //}
    
    cout << " Words\t\t\t   Query TFIDF\n\n";
    for (int i = 0; i < tfDic.size(); i++)
    {   
        cout << fixed <<" "<<Qwords[i] << "\ttf-raw: " << tfDic[Qwords[i]].size() << "\ttf-wght: " << 1 + log10(tfDic[Qwords[i]].size()) << "\tdf: " << vec[i].size() << "\t\tidf: " << ((vec[i].size()==0) ? (0) : ( log10( double (fileList.size()) / (vec[i].size()) ) ) ) << endl;
    }
    cout << endl;

    
    for (int i = 0; i < fileList.size(); i++)
    {
        cout << " File (" << fileList[i] << ") Words: " << fileWords[fileList[i]][0].wordCount << endl;
    }
     
    //Repeat of a query words in a Doc
    for (int k = 0; k < fileList.size(); k++)
    { 
        for (int i = 0; i < Qwords.size(); i++)
        {
            int counter=0;
            for (int j = 0; j < dictionary[Qwords[i]].size(); j++)
            {
                if (dictionary[Qwords[i]][j].file == fileList[k])
                {
                    counter++;
                }
            }
            postinglist obj;
            obj.wordFileCount = counter;
            wordFileCount[Qwords[i]][fileList[k]].push_back(obj);
        }
    }
    
    //print
    invertedIndex docWeig;
    vector<float> pductvec;
    vector<float> docpduct;
    for (int i = 0; i < fileList.size(); i++)
    {
        
        cout << endl << " Words\t\t\tDoc ("<<fileList[i]<<") TFIDF (Weight: " << docWeight[fileList[i]][0].docWei <<")\n\n";
        for (int j = 0; j < Qwords.size(); j++)
        {
            float normalize = ((docWeight[fileList[i]][0].docWei == 0) ? (0) : (((wordFileCount[Qwords[j]][fileList[i]][0].wordFileCount == 0) ? (0) : (1 + log10(wordFileCount[Qwords[j]][fileList[i]][0].wordFileCount))) / (docWeight[fileList[i]][0].docWei)));
            float wordpduct = normalize*((vec[j].size() == 0) ? (0) : (log10(double(fileList.size()) / (vec[j].size()))));
            pductvec.push_back(wordpduct);
            cout<< fixed <<" "<< Qwords[j]<<"\ttf-raw: " << wordFileCount[Qwords[j]][fileList[i]][0].wordFileCount << "\ttf-wght: " << ((wordFileCount[Qwords[j]][fileList[i]][0].wordFileCount==0 ) ? (0) : (1 + log10(wordFileCount[Qwords[j]][fileList[i]][0].wordFileCount))) <<"\tNormalize: "<< normalize <<"\tPduct: "<< wordpduct  << endl;
        }
        
        float t = 0;
        for (int k = 0; k < pductvec.size(); k++)
        {   
             t+= pductvec[k];
        }
        pductvec.clear();
        docpduct.push_back(t);
        cout << endl<< " Final Doc ("<<fileList[i]<<") pduct: " << docpduct[i] << endl;
    }
    //file to pduct connect
    map<float, string> filetopduct;
    int docpductsize = docpduct.size();
    for (int i = 0; i < fileList.size(); i++)
    {
        //dont address 0 pduct docs in map
        if (docpduct[i] != 0)
        {
            filetopduct[docpduct[i]] = fileList[i];
        }
        else
            docpductsize--;
    }
    
    //sort and print filename of lowest doc pduct
    sort(docpduct.begin(), docpduct.end(), greater<float>());
    cout << endl << " Final Result: ";
    //shows only top 10 result
    if (docpductsize == 0)
    {
        cout << endl << " Noting found !!!";
    }
    else
    {
        for (int i=0; i< ((docpductsize >10) ? (10) : (docpductsize)); i++)
            {   
                cout << endl << i+1 <<": "<<filetopduct[docpduct[i]];
            }
    }
}

int main(int argc, char* argv[])
{
    invertedIndex Data;

    for (int i = 1; i < argc; i++)
    {
        Data.addFile(argv[i]);
    }

    do
    {
        int choice = 0;
        cout << endl <<"1: Show Files\n2: Add File\n3: Inverted Index Retrieval\n4: TF-IDF Retrieval\n5: Exit\nEnter Choice: ";
        cin >> choice;
        system("CLS");
        cin.clear();
        cin.ignore();
        switch (choice)
        {
        case 1:
        {
            Data.showFiles();
            break;
        }
        case 2:
        {
            cout << "Enter File Name: ";
            string name;
            cin >> name;
            Data.addFile(name);
            break;
        }
        case 3:
        {
            cout << "To Retrieve More Than 1 Word Use & As Separator And Operator For Example: Antony&Brutus\n";
            cout << "Enter Word/s: ";
            string word;
            cin >> word;
            word = stringtolower(word);
            vector <string> words;
            string delimiter = "&";

            int position = 0;
            while ((position = word.find(delimiter)) != string::npos)
            {
                string token = word.substr(0, position);
                words.push_back(token);
                word.erase(0, position + delimiter.length());
            }
            words.push_back(word);

            cout << endl;
            for (int i = 0; i < words.size(); i++)
                Data.retrieval(words[i]);
            cout << endl;
            if (words.size() > 1)
                Data.finalResult();
            break;
        }
        case 4:
        {
            cout << "To Retieve Query Enter Like Example: how are you\n";
            cout << "Enter Query: ";
            string Query;
            getline(cin , Query);
            Query = stringtolower(Query);
            vector <string> Qwords;
            /*replace(Query.begin(), Query.end(), ' ', '-');
            string del = " ";

            int position = 0;
            while ((position = Query.find(del)) != string::npos)
            {
                string token = Query.substr(0, position);
                Qwords.push_back(token);
                Query.erase(0, position + del.length());
            }
            Qwords.push_back(Query);

            cout << endl;
            for (int i = 0; i < Qwords.size(); i++)
                cout<<Qwords[i]<<"\n";*/
            istringstream ss(Query);
            string term; 
            while (ss >> term)
            {
                Qwords.push_back(term);
            }
            cout << endl;
            Data.tfidf(Qwords);
            cout << endl;
            break;
        }
        case 5:
        {
            return 0;
            break;
        }
        default:
        {
            cout << "Try Again!\n";
            break;
        }
        }
    } while (1);
}
