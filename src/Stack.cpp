#include "Stack.hpp"

//ClassImp(Stack)

std::map< std::string, int> Stack::canvasNameNumberOfInstance;

Stack::Stack( std::string _name ) : name( _name ){
    Init();
}

Stack::Stack(std::string fileName, std::string histoName) : name(histoName){
    Init();
    TFile::Open( fileName.c_str() );
    push_back( name );
}

Stack::Stack( std::map< std::string, std::string > fileAndLabel, std::string histoName ) {
    Init();
    Load( fileAndLabel, histoName );
}
	  
Stack::Stack(TH1 *h){
    Init();
    push_back(h);
}

std::string Stack::indexName( std::string _name ){
    canvasNameNumberOfInstance[ _name ]++;
    
    std::string nameIndex = _name;
    int nInstance = canvasNameNumberOfInstance[ _name ];

    std::stringstream ss;
    if( nInstance > 1 ){
        std::string res;
        ss << "<" << nInstance << ">";
    }
    return nameIndex + ss.str();
}

void Stack::Init(){
    title = name;
    
    name = indexName(name);

    frame = NULL;
    can = NULL;
    leg = new TLegend(0.7, 0.8, 1, 1);
    leg -> SetFillColor(0);
    leg -> SetLineColor(0);
    _displayLegend = true;
    gStyle->SetOptStat(0);
    labelSizeX = 0;
    labelSizeY = 0;
    verbose = 0;
}


// TH1* Stack::push_back(std::string filename, std::string histoOrTTreeOrTcanvasName, std::string varexp, std::string selection, std::string option, Long64_t nentries, Long64_t firstentry){

//   TKey *key;
//   TIter nextkey(file -> GetListOfKeys());
//   while ((key = (TKey*)nextkey())) {
//     const char *classname = key->GetClassName();
//     TClass *cl = gROOT->GetClass(classname);
//     if( strcmp( key -> GetName(), histoOrTTreeOrTcanvasName) == 0){
//       if (!cl) continue;
//       if (cl->InheritsFrom(TH1::Class())) {
	
//       }else if (cl->InheritsFrom(TTree::Class())) {
	
//       }else if (cl->InheritsFrom(TCanvas::Class())) {
	
//       }
//     }
//   }
//   return NULL;

// }

TH1* Stack::push_back(std::string filename, std::string treename, std::string varexp, std::string selection, std::string option, Long64_t nentries, Long64_t firstentry){
    TFile* file = new TFile( filename.c_str() );
    if( file == NULL ){
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "No file named : " << filename << std::endl;
        return NULL;
    }
    TTree* tree = (TTree*) file -> Get( treename.c_str() );
    if( tree == NULL ){
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "File : " << filename << " doesn't have a TTree called : " << treename << std::endl;
        return NULL;
    }
    return push_back(tree,varexp, selection, option, nentries, firstentry);
}

std::string replacePattern(std::string str, const std::string& oldPattern, const std::string& newPattern){
    std::string result = str;
    size_t pos = 0;
    while((pos = result.find(oldPattern, pos)) != std::string::npos)
        {
            result.replace(pos, oldPattern.length(), newPattern);
            pos += newPattern.length();
        }

    return result;
}

TH1* Stack::push_back(TTree* tree, std::string varexp, std::string selection, std::string option, Long64_t nentries, Long64_t firstentry){
    if( tree == NULL ) {
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "No pointer on tree !" << std::endl;
        return NULL;
    }

    tree -> Draw( varexp.c_str(), selection.c_str(), (option+"goff").c_str(), nentries, firstentry );

    std::string varexpTmp = replacePattern(varexp,"::","__"); // If not this line, next line does not work properly
    size_t posSecondVariable = varexpTmp.find_first_of(':'); // Check if it is 2D histogram
  
    Size_t pos = varexp.find(">>"); // Check if the user specified an histogram
    if( pos == std::string::npos ){
        pos = varexp.length();
    }
  
    std::string histoTitleX, histoTitleY;
    std::string histoName;


    bool is2D = false;
    if( posSecondVariable != std::string::npos ){
        is2D = true;
        histoTitleX = varexp.substr(posSecondVariable+1, pos - posSecondVariable - 1);
        histoTitleY = varexp.substr(0,posSecondVariable);
    }else{
        histoTitleX = varexp.substr(0,pos);
        histoTitleY = "# events";
    }
      
    if( pos != varexp.length() ){
        std::string histo = varexp.substr(pos+2, varexp.length());
        histoName = histo;
        pos = histo.find_first_of('('); // Check if user specified a range for the histogram
        if( pos != std::string::npos ){
            histoName = histo.substr(0,pos);
        }
    }else{
        histoName = "htemp";
    }

    TH1* theHisto = (TH1*) gDirectory -> Get( histoName.c_str() );
    if( theHisto == NULL ){
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "No histo found by gDirectory -> Get : " << histoName << std::endl;
        return NULL;
    }

    drawingOption[ theHisto ] += option;

    theHisto -> SetTitle( histoName.c_str() );
    theHisto -> GetXaxis() -> SetTitle( histoTitleX.c_str() );
    theHisto -> GetYaxis() -> SetTitle( histoTitleY.c_str() );
    if( titleX == "" ) titleX = histoTitleX;
    if( titleY == "" ) titleY = histoTitleY;

    push_back(theHisto);

    return theHisto;

}
    
Stack::~Stack(){
    if( leg ){
        delete leg;
        leg = NULL;
    }

    if( can ){
        delete can;
        can = NULL;
    }

    if( frame ){
        delete frame;
        frame = NULL;
    }

    std::cout << "Destroying stack : " << name << std::endl;
}


void Stack::Load( std::map< std::string, std::string > fileAndLabel, std::string histoName ){
    for( std::map< std::string, std::string>::iterator it = fileAndLabel.begin(); it != fileAndLabel.end(); ++it ){
        new TFile( (it -> first).c_str() );
        push_back( histoName, it -> second );
    }
}
    
float Stack::getMaximumInRange(TH1* h, float xinf, float xsup){
    int bin = h -> FindFixBin(xinf);
    int nBins = h -> GetNbinsX();
    float max = 0;
    float content;
    if(bin < 1) bin = 1;
    while( bin <= nBins && h -> GetBinLowEdge(bin + 1) < xsup){
        content = h -> GetBinContent( bin );
        if( content > max){
            max = content;
        }
        bin++;
    }
    return max;
}

    
TH1* Stack::push_back(std::string str, std::string label){
    TH1 *h = (TH1*)gDirectory->Get(str.c_str());
    if(!h) {
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "No such histo in file" << std::endl;
        return NULL;
    }
    push_back(h,label);
    return h;
}

TH1* Stack::push_back(TH1* h, std::string label){
    if(!h) {
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "histo pointer is null" << std::endl;
        return NULL;
    }

    vec.push_back(h);

    if( label == "" ) label = h -> GetTitle();
    //  if( label != "htemp" )
    mapLegEntry[h] = leg -> AddEntry(h, label.c_str() ,"l");

    return h;
}


TGraph* Stack::push_back(TGraph* gr, std::string tlegendLabel ){
    if(!gr) {
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "graph pointer is null" << std::endl;
        return NULL;
    }

    vecGraph.push_back(gr);
  
    if( tlegendLabel != "" ) mapLegEntry[gr] = leg -> AddEntry(gr, tlegendLabel.c_str() ,"lp");
    return gr;
}

TLine* Stack::push_back(TLine* line, std::string tlegendLabel ){
    if(!line) {
        std::cout << "WARNING in Stack::push_back" << std::endl;
        std::cout << "line pointer is null" << std::endl;
        return NULL;
    }

    vecLine.push_back( line );
  
    if( tlegendLabel != "" ) mapLegEntry[line] = leg -> AddEntry(line, tlegendLabel.c_str() ,"l");
    return line;
}

TF1* Stack::push_back(TF1* f, std::string label ){
    if(!f) {
        std::cout << "line pointer is null" << std::endl;
        return NULL;
    }

    vecTF1.push_back( f );
  
    if( label != "" ) mapLegEntry[f] = leg -> AddEntry(f, label.c_str() ,"l");
    return f;
}

void Stack::normalize(float xinf, float xsup){
    int N = vec.size();
    if(N == 0) return;

    ComputeLimits();
      
    if( xinf == 0 && xsup == 0){
        xinf = limitsX.first;
        xsup = limitsX.second;
    }

    float max;

    for(int i = 0;i < N; i++){
        max = getMaximumInRange( vec[i], xinf, xsup);
        if( max != 0 ){
            vec[i] -> Scale( 1. / max );
        }
    }
}

void Stack::normalizeToBiggest(float xinf, float xsup){
    int N = vec.size();
    if(N == 0) return;

    ComputeLimits();
      
    if( xinf == 0 && xsup == 0){
        xinf = limitsX.first;
        xsup = limitsX.second;
    }

    float max;

    for(int i = 0;i < N; i++){
        float m = getMaximumInRange( vec[i], xinf, xsup);
        if( m > max ) max = m;
    }

    if( max > 0 ){
        for(int i = 0;i < N; i++){
            vec[i] -> Scale( 1. / max );
        }
    }
}

void Stack::setTitle( std::string _title ){
    title = _title;
}

std::string Stack::getTitle(){
    return title;
}

void Stack::setLabels( std::string labelX, std::string labelY ){
    setXLabel(labelX);
    setYLabel(labelY);
}

void Stack::setXLabel( std::string label ){
    titleX = label;
}

void Stack::setYLabel( std::string label ){
    titleY = label;
}

void Stack::setLabelsSize( float _sizeX, float _sizeY ){
    setXLabelSize(_sizeX);
    setYLabelSize(_sizeY);
}

void Stack::setXLabelSize( float _sizeX ){
    labelSizeX = _sizeX;
}

void Stack::setYLabelSize( float _sizeY ){
    labelSizeY = _sizeY;
}

void Stack::setTitlesSize( float _sizeX, float _sizeY ){
    setXTitleSize(_sizeX);
    setYTitleSize(_sizeY);
}

void Stack::setXTitleSize( float _sizeX ){
    labelSizeX = _sizeX;
}

void Stack::setYTitleSize( float _sizeY ){
    labelSizeY = _sizeY;
}

void Stack::draw2D(){
    if( debug > 0 ) std::cout << "Stack::draw2D()" << std::endl;
    int N = vec.size();

    if(debug > 0) std::cout << "N : " << N << std::endl;
    if( N  == 0 ) return;

    can -> cd();
    if( frame == NULL){
        frame = (TH2*) vec[0] -> Clone("frame");
        frame -> Reset();
        frame -> GetXaxis() -> SetTitle( titleX.c_str() );
        frame -> GetYaxis() -> SetTitle( titleY.c_str() );
        if(strcmp(frame -> GetTitle(), "htemp") == 0 && strcmp(frame -> GetTitle(), "Default") == 0) frame -> SetTitle( title.c_str() );
    }

    frame -> Draw();

    for(int i = 0; i<N;i++){
        vec[i] -> SetMarkerColor( getColor(i) );
        vec[i] -> SetLineColor( vec[i] -> GetMarkerColor() );

        drawingOption[ vec[i] ] += "same";
        if(N == 1){
            std::size_t pos = drawingOption[ vec[i] ].find("black");
            if( pos == std::string::npos ) drawingOption[ vec[i] ] += "colz";
        }
        vec[i] -> Draw( drawingOption[ vec[i] ].c_str() );
    }

    int Ngraph = vecGraph.size();
    for(int i = N;i<Ngraph+N;i++){
        if(debug > 9) std::cout << "drawing graph # : " << i-N << " with # : " << vecGraph[i-N] -> GetN() << " points in Stack : " << name << std::endl;
        drawGraph( vecGraph[i-N], i );
    }

    int Nlines = vecLine.size();
    for(int i = N + Ngraph;i<Ngraph+N+Nlines;i++){
        drawLine( vecLine[i-(N+Ngraph)] );
    }
}


void Stack::setFrame( float infX, float supX, float infY, float supY ){
    if(frame) delete frame;

    if( supX < infX ){
        std::cerr << "WARNING: borne sup X cannot be smaller than borne inf X" << std::endl;
        return;
    }

    if( supY < infY ){
        std::cerr << "WARNING: borne sup Y cannot be smaller than borne inf Y" << std::endl;
        return;
    }
  
    std::string theName = indexName("frame");
    frame = new TH2F(theName.c_str(), title.c_str(),10000, infX, supX, 10000, infY, supY);
    
    frame -> GetXaxis() -> SetTitle( titleX.c_str() );
    frame -> GetYaxis() -> SetTitle( titleY.c_str() );
}
    
void Stack::setDrawingOption( TObject* obj, std::string theDrawingOption ){
    drawingOption[ obj ] = theDrawingOption;
}

void Stack::draw1D(){
    if( debug > 0 ) std::cout << "Stack::draw1D()" << std::endl;
    int nHisto = vec.size();
    int Ngraph = vecGraph.size();
  
    if( frame == NULL ){
        ComputeLimits();
        frame = new TH2F( indexName("frame").c_str(), title.c_str(),1000, limitsX.first, limitsX.second,1000, limitsY.first, limitsY.second + topMarginFactor * (limitsY.second - limitsY.first) );
        frame -> GetXaxis() -> SetTitle( titleX.c_str() );
        frame -> GetYaxis() -> SetTitle( titleY.c_str() );
    }

    can -> cd( );
    frame -> Draw();
      
    for(int i = 0;i<nHisto;i++){
        vec[i] -> SetLineColor( getColor(i) );
        vec[i] -> SetLineWidth(2);
        if( nHisto == 1 ) vec[i] -> SetFillColor(18);
        drawingOption[ vec[i] ] += "same";
        vec[i] -> Draw( drawingOption[ vec[i] ].c_str() );
    }

    // Add all graphs on the plot
    for(int i = nHisto;i<Ngraph+nHisto;i++) drawGraph( vecGraph[i-nHisto], i );

    int Nlines = vecLine.size();
    for(int i = nHisto + Ngraph;i<Ngraph+nHisto+Nlines;i++){
        drawLine( vecLine[i-(nHisto+Ngraph)] );
        vecLine[i-(nHisto+Ngraph)] -> SetLineColor( getColor(i) );
    }

    int NTF1 = vecTF1.size();
    for(int i = 0; i<NTF1;i++){
        vecTF1[i] -> SetLineColor( getColor(i+Ngraph+nHisto+Nlines) );
        vecTF1[i] -> Draw("same");
    }
}

void Stack::drawLine( TLine* line ){
    if( line -> GetX1() == line -> GetX2() ){
        line -> SetY1( frame -> GetYaxis() -> GetXmin() );
        line -> SetY2( frame -> GetYaxis() -> GetXmax() );
    }

    if( line -> IsHorizontal() ){
        line -> SetX1( frame -> GetXaxis() -> GetXmin() );
        line -> SetX2( frame -> GetXaxis() -> GetXmax() );
    }
  
    line -> SetLineWidth(3);
    line -> SetLineStyle(4);
    line -> Draw("same");
}

void Stack::setLegendPosition( double xInf, double yInf, double xSup, double ySup ){
    leg -> SetX1( xInf );
    leg -> SetY1( yInf );
    leg -> SetX2( xSup );
    leg -> SetY2( ySup );
}

void Stack::drawGraph( TGraph* gr, int objectNumber ){
    // i user has not specified option 'customColor', then set a style
    bool isCustomColor = checkIfCustomColor( gr );

    if( isCustomColor == false ){ 
        gr -> SetLineColor( getColor(objectNumber) );
        gr -> SetMarkerStyle(20);
        gr -> SetMarkerColor( getColor(objectNumber) );
        gr -> SetLineWidth(2);
    }
    gr -> Draw( "p" );

    // Plot functions associated with graphs if any
    int nFunctions = gr -> GetListOfFunctions() -> GetEntries();
    for(int i = 0; i < nFunctions; i++){
        gr -> GetListOfFunctions() -> At(i) -> Draw("same");
    }
  
}

Stack* Stack::draw(TVirtualPad* customCanvas){
    if( customCanvas == NULL ) return draw();
    if( can != NULL ) delete can;
    can = (TPad*) customCanvas;

    can -> SetTitle(title.c_str());

    return draw();
}

Stack* Stack::draw(){
    if( can == NULL ) can = new TCanvas(("can_"+name).c_str(), name.c_str());

    int N = vec.size();
    int Ngraph = vecGraph.size();
    if(N == 0 && Ngraph == 0) return this;

    bool plotLegend = checkIfDrawLegend();

    if( N == 0 ) draw1D(); 
    else if( vec[0] -> InheritsFrom("TH2") ) draw2D();
    else draw1D();

    if( _displayLegend && leg -> GetListOfPrimitives() -> GetSize() > 0 && plotLegend ) leg -> Draw("same");

    if( labelSizeX != 0 ) frame -> GetXaxis() -> SetLabelSize( labelSizeX );
    if( labelSizeY != 0 ) frame -> GetYaxis() -> SetLabelSize( labelSizeY );

    frame -> GetXaxis() -> SetNdivisions(507,true);
    frame -> GetYaxis() -> SetNdivisions(507,true);

    frame -> GetXaxis() -> SetLabelSize(0.07);
    frame -> GetYaxis() -> SetLabelSize(0.07);

    if( titleX != "" ){
        frame -> GetXaxis() -> SetTitleSize(0.08);
        //    frame -> GetXaxis() -> SetTitleOffset(1.4);
        gPad -> SetBottomMargin(0.2);
    }
  
    if( titleY != "" ){
        frame -> GetYaxis() -> SetTitleSize(0.08);
        gPad -> SetLeftMargin(0.2);
    }

    gPad -> SetGridx();
    gPad -> SetGridy();
    // gPad->RedrawAxis();
    return this;
}

void Stack::editLegendEntry(TH1* h, std::string str){
    if( mapLegEntry[h] == NULL){
        std::cout << "No TLegendEntry for this histogram" << std::endl;
        return;
    }
    mapLegEntry[h] -> SetLabel( str.c_str() );
}

void Stack::ComputeLimits(){
    int N = vec.size();
    int Ngraph = vecGraph.size();
    Double_t infX, supX, infY, supY;
      
    if(N == 0 && Ngraph == 0) return;
    for(int i = 0;i < N + Ngraph; i++){
        // Loop through histograms
        if(i < N ){
            infX = vec[i] -> GetBinLowEdge(1);
            supX = vec[i] -> GetBinLowEdge( vec[i] -> GetNbinsX() + 1);

            infY = vec[i] -> GetMinimum();
            supY = vec[i] -> GetMaximum();
            if( verbose > 0 ){
                std::cout << "Histo : " << i << std::endl;
                std::cout << "infX : " << infX << std::endl;
                std::cout << "infY : " << infY << std::endl;
                std::cout << "supX : " << supX << std::endl;
                std::cout << "supY : " << supY << std::endl;
            }
        }
        // Loop through TGraphs
        else {
            
            vecGraph[ i-N ] -> ComputeRange( infX, infY, supX, supY );

            if( vecGraph[ i-N ] -> GetN() > 1 ){
                infX -= 0.1 * (supX - infX);
                supX += 0.1 * (supX - infX);

                infY -= bottomMarginFactor * (supY - infY);
            }else{
                infX -= 0.01 * std::abs(infX);
                supX += 0.01 * std::abs(supX);
                infY -= 0.01 * std::abs(infY);
            }
        }

        if(i == 0){
            limitsX.first = infX;
            limitsX.second = supX;

            limitsY.first = infY;
            limitsY.second = supY;
        }else{
            if(limitsX.first > infX) limitsX.first = infX;
            if(limitsX.second < supX ) limitsX.second = supX;

            if(limitsY.first > infY ) limitsY.first = infY;
            if(limitsY.second < supY ) limitsY.second = supY;
        }
    }
}

int Stack::getColor(int n, bool fill){
    int N = 9;
    int col[]= {1, 4, 2, 8, 11, 7, 46, 6, 49};
    int color= n<N?col[n]:5+n;
    if (n==0 && fill) color=38;
    return color;
}

void Stack::write(){
    if(can) can -> Write();
}

void Stack::write( std::string nameWithoutExtension, std::string extension ){
    if( can == NULL ){
        std::cout << "can pointer is null, cannot write !!!!" << std::endl;
        return;
    }

    if( extension == "" ){
        can -> SaveAs( (nameWithoutExtension + ".root").c_str() );
        can -> SaveAs( (nameWithoutExtension + ".eps").c_str() );
    }else{
        can -> SaveAs( (nameWithoutExtension + "." + extension ).c_str() );
    }
}

bool Stack::checkIfDrawLegend(){
    // Look if one of the drawing option is "nolegend", if yes return false and suppress it from the option list

    bool plotLegend = true;
    for( std::map< TObject*, std::string >::iterator it = drawingOption.begin(); it != drawingOption.end(); it++){
        std::size_t pos = (it->second).find("nolegend");
        if( std::string::npos != pos ) {
            (it -> second).replace(pos,8,"");
            plotLegend = false;
        }
    }

    return plotLegend;
}

bool Stack::checkIfCustomColor( TGraph* gr ){
    // Check if the graph has its own style (via the 'customColor' option)
    std::size_t pos = drawingOption[ gr ].find("customColor");
    if( std::string::npos != pos ) {
        drawingOption[ gr ].replace(pos,11,"");
        return true;
    }

    return false;
}

void Stack::setLegendDisplay( bool _display ){
    _displayLegend = _display;
}

TLine* Stack::pushVerticalLine( float x, std::string tlegendLabel ){
    TLine *line = new TLine(x,0,x,1);
    push_back( line, tlegendLabel );
    return line;
}

TLine* Stack::pushHorizontalLine( float y, std::string tlegendLabel ){
    TLine *line = new TLine();
    line -> SetY1( y );
    line -> SetHorizontal();
    push_back( line, tlegendLabel );
    return line;
}

int Stack::getNHistos(){
    return vec.size();
}

TH1* Stack::getHisto(std::string name){
    for(int i = 0;i<vec.size();i++){
        if(std::string(vec[i] -> GetName()) == name) return vec[i];
    }

    std::cout << "Histo: "<< name << " not found !" << std::endl;
    return NULL;
}

TH1* Stack::at(int i){
    if(i >= vec.size()){
        std::cout << "Error: Element ("<< i << ") out of bounds (" << vec.size() << ") !"  << std::endl;
        return NULL;
    }

    return vec[i];
}
