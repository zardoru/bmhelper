
#pragma once

#include <vector>
#include "common.h"
#include <wx/file.h>

// データの簡易尻洗い座
// ほぼRIFFと同じ階層型チャンクシステム。リトルエンディアン。
// RIFFでいうところのRIFF/LISTチャンクは大文字英数で始まるチャンク名(ノード名)で表す。それ以外は通常チャンク
// 同じ親の下に同じノード名のノードが複数あってもよい


typedef unsigned long NodeName;
typedef unsigned long NodeSizeType;


#define StringToNodeName(str_literal) (CharsToNodeName(str_literal[0], str_literal[1], str_literal[2], str_literal[3]))
#define CharsToNodeName(c1, c2, c3, c4) (((NodeSizeType)(c1)&0xff) | (((NodeSizeType)(c2)&0xff)<<8) | (((NodeSizeType)(c3)&0xff)<<16) | (((NodeSizeType)(c4)&0xff)<<24))


class wxFile;


class TreeNode : public std::vector<TreeNode>{
	NodeName name;
	char *data;
	size_t data_size;
public:
	TreeNode() : name(0), data(0), data_size(0){}
	TreeNode(const TreeNode &src) : std::vector<TreeNode>(src), name(src.name),
		data(src.data_size ? new char[src.data_size] : 0), data_size(src.data_size)
	{
		if (data_size) memcpy(data, src.data, data_size);
	}
	TreeNode(NodeName _n) : name(_n), data(0), data_size(0){}
	~TreeNode(){ clear_data(); }
	NodeName get_name() const{ return name; }
	void set_name(NodeName n){ name = n; }
	bool is_list() const{
		int c = (name & 0x000000ff);
		return c>='A' && c<='Z';
	}
	void clear_data(){ if (data) delete[] data; data_size=0; }
	size_t get_data_size() const{ return data_size; }
	int get_data(void *dst, size_t dst_size) const{
		if (!data) return 0;
		if (data_size > dst_size) return -1;
		memcpy(dst, data, data_size);
		return data_size;
	}
	void set_data(const void *src, size_t src_size){
		if (!src_size) return;
		clear_data();
		data = new char[data_size = src_size];
		memcpy(data, src, data_size);
	}
private:
	friend class TreeDocument;
	void read(wxFile &file){
		clear_data();
		file.Read(&name, sizeof(NodeName));
		NodeSizeType size;
		file.Read(&size, sizeof(NodeSizeType));
		if (is_list()){
			// リスト系のアレなら子要素を読み込む
			wxFileOffset inner_head = file.Tell();
			wxFileOffset inner_tail = inner_head + size;
			while (file.Tell() < inner_tail){
				push_back(TreeNode());
				back().read(file);
			}
			file.Seek(inner_tail);
		}else{
			// リストでなければ自分のdataに読み込む
			if (size){
				data = new char[data_size = size];
				file.Read(data, data_size);
			}
		}
	}
	void write(wxFile &file) const{
		file.Write(&name, sizeof(NodeName));
		if (is_list()){
			file.Seek(sizeof(NodeSizeType), wxFromCurrent);
			wxFileOffset inner_head = file.Tell();
			for (std::vector<TreeNode>::const_iterator i=begin(); i!=end(); i++){
				i->write(file);
			}
			wxFileOffset inner_tail = file.Tell();
			NodeSizeType size = inner_tail - inner_head;
			file.Seek(inner_head-sizeof(NodeSizeType));
			file.Write(&size, sizeof(NodeSizeType));
			file.Seek(inner_tail);
		}else{
			NodeSizeType size = data_size;
			file.Write(&size, sizeof(NodeSizeType));
			if (data_size) file.Write(data, data_size);
		}
	}
};


class TreeDocument{
	TreeNode root;
public:
	void load(const wxString &filename){
		wxFile file(filename, wxFile::read);
		root.read(file);
	}
	void save(const wxString &filename) const{
		wxFile file(filename, wxFile::write);
		root.write(file);
	}
	TreeNode &get_root(){ return root; }
	const TreeNode &get_root() const{ return root; }
};





