#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <iostream>
#include <unordered_map>

#define MAX_WORD_LEN 32

class trie_node{
	private:
		int status;
		std::unordered_map<char,trie_node *> children;

        void _show_words(char word[MAX_WORD_LEN],int len){
            if(this->get_status() == 1){
                word[len] = '\0';
                std::cout<<word<<std::endl;
            }
            for(auto one : children){
                if(len > MAX_WORD_LEN) return;
                word[len] = one.first;
                one.second->_show_words(word,len+1);
            }
        }

	public:
		trie_node(){
			status = 0;
		}
		// @brief:set status 1
		void set_status(){ this->status = 1; }

		// @brief:get status value
		int get_status(){ return status; }

		// @brief:add word to trie
        bool add_word(const char *word,int len){
            trie_node * trie = this;
            char c;
            for(int i = 0;i<len;i++){
                // 不区分大小写
                if(word[i] >= 'A' && word[i] <= 'Z'){
                    c = word[i] + 'a' - 'A';
                }
                else{
                    c = word[i];
                }
                if(trie->children.find(c)==trie->children.end()){
                    trie->children[c] = new trie_node();
                }
                trie = trie->children[c];
            }
            trie->set_status();
            return true;
        }

        // @brief:add per line in file to trie
        bool load_file(const char *path){
            if(path == NULL){
                std::cout<<"path is empty"<<std::endl;
                return false;
            }
            FILE *fp = fopen(path,"r");
            if(fp == NULL){
                std::cout<<"no such file "<<path<<std::endl;
                return false;
            }

            char c;
            char str[MAX_WORD_LEN+1];
            int len = 0;
            while((c = fgetc(fp)) != EOF){
                if(c == ' ' || c == '\t' || c == '\r'){
                    continue;
                }
                if(c == '\n' && len > 0 && len <= MAX_WORD_LEN){
                    str[len] = '\0';
                    this->add_word(str,len);
                    len = 0;
                    continue;
                }
                str[len++] = c;
            }
            fclose(fp);
            return true;
        }

        // @brief:judge word in trie
        bool is_exist(const char *word,int len){
            trie_node * trie = this;
            for(int i = 0;i<len;i++){
                if(trie->children.find(word[i])==trie->children.end()){
                    return false;
                }
                trie = trie->children[word[i]];
            }
            if(trie->get_status() == 0){
                return false;
            }
            return true;
        }

        // @brief:print all exist word to stdout
        void show_words(){
            char word[MAX_WORD_LEN + 1];
            this->_show_words(word,0);
        }
};

#endif
