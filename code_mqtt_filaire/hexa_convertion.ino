int Strtoint(int index){
  for(int y = 0; y < sizeof(check_hex); y++ ){
    if(tab_lum[index] == check_hex[y]){
      return y;
    }
  }
}