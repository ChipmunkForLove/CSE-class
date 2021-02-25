#include "cache.h"
#include "csapp.h"

//used linkedlist for cache, replacement policy is FIFO
//always insert as first element of linked list (insert at header.next)
//always remove the last element of linked list when replacement happens
void cache_replacement_policy(){
	cache_block* curr = header.next;
	cache_block* prev = NULL;
	
	for(;curr->next!=NULL;curr=curr->next){
		prev=curr;
	}
	header.totallength -= (curr-> contentLength);
	free(curr);
	prev->next = NULL;
}

int add_cache_block(char* uri,char* content,char* response,int contentLength){

	/// use cache replacement policy if the proxy cache is full.
	/// you can use any cache replacement policy such as FIFO
        if(find_cache_block(uri)!=NULL){
		//already exist
		return 1;
	}	
	if(contentLength>MAX_OBJECT_SIZE){
		//too big!
		return 1;
	}
	cache_block* new = (cache_block*)Malloc(sizeof(cache_block));
	//make new cacheblock
	if(header.totallength<MAX_CACHE_SIZE){
		//there is still place for new cache block
		strcpy(new->uri,uri);
		strcpy(new->content,content);
		strcpy(new->response,response);
		new->contentLength=contentLength;
		new->next=header.next;
		header.next=new;//insert between header and header->next
		header.totallength += contentLength;
		return 0;
	}
	else{
		//cache is full - use replacement policy
		while((header.totallength+contentLength)>MAX_CACHE_SIZE){
			cache_replacement_policy();
		}
		strcpy(new->uri,uri);
		strcpy(new->content,content);
		strcpy(new->response,response);
		new->contentLength=contentLength;
		new->next=header.next;
		header.next=new;
		header.totallength += contentLength;
		return 0;
	}
	//if it is cached well, return 0, or return 1
	return 1;
}
cache_block* find_cache_block(char* uri){
	cache_block* curr= header.next;
	for( ;curr!=NULL;curr=curr->next){
		if(!strcmp(curr->uri,uri)){
			return curr;
		}
	}
	return NULL;
}
