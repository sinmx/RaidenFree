#include "bmobuser.h"
#include "bmobSDKinit.h"
#include "cocos2d.h"
#include "network/HttpRequest.h"

USING_NS_CC;

string BmobUser::__USER_FILE = "cur_user.xml";
BmobUser* BmobUser::currentUser = NULL;

BmobUser* BmobUser::getCurrentUser(){
	if (currentUser != NULL)
	{
		/* code */
		return currentUser;
	}

	string id = UserDefault::getInstance()->getStringForKey("user_id");
	string pwd = UserDefault::getInstance()->getStringForKey("user_pwd");
	string name = UserDefault::getInstance()->getStringForKey("user_name");
	string session = UserDefault::getInstance()->getStringForKey("user_session");

	if (id.empty() || name.empty() || session.empty())
	{
		/* code */
		return NULL;
	}

	currentUser = new BmobUser();
	currentUser->m_objectId = id;
	currentUser->m_username = name;
	currentUser->m_session = session;
	currentUser->setPassword(pwd);

	return currentUser;
}

void BmobUser::logOut(){
	if (currentUser == NULL)
	{
		/* code */
		return ;
	}

	UserDefault::getInstance()->setStringForKey("user_id","");
	UserDefault::getInstance()->setStringForKey("user_name","");
	UserDefault::getInstance()->setStringForKey("user_pwd","");
	UserDefault::getInstance()->setStringForKey("user_session","");

	delete currentUser;
	currentUser = NULL;
}

BmobUser::BmobUser():
BmobObject(BmobSDKInit::USER_TABLE){

}

BmobUser::~BmobUser(){

}

void BmobUser::setPassword(string password){
	//encrpty
	this->m_password = password;
}

string BmobUser::getPassword(){
	return this->m_password;
}

void BmobUser::signUp(BmobSaveDelegate* delegate){
	if (m_username.empty() || m_password.empty())
	{
		/* code */
		return ;
	}

	this->m_url =  BmobSDKInit::USER_URL;

	_opType = HTTP_OP_Type::_bHTTP_SAVE;

	this->m_pSaveDelegate = delegate;

   /**
	* save user
	*/
	UserDefault::getInstance()->setStringForKey("user_name",m_username);
	UserDefault::getInstance()->setStringForKey("user_pwd",m_password);

	this->send(network::HttpRequest::Type::POST);
}

void BmobUser::login(BmobSaveDelegate* delegate){
	if (this->m_username.empty() || this->m_password.empty())
	{
		/* code */
		return ;
	}

	this->m_url = BmobSDKInit::LOGIN_URL  + "?username=" + \
					this->m_username + "&password=" + this->m_password;
	this->clear();

	/**
	* save user
	*/
	UserDefault::getInstance()->setStringForKey("user_name",m_username);
	UserDefault::getInstance()->setStringForKey("user_pwd",m_password);

	_opType = HTTP_OP_Type::_bHTTP_SAVE;

	this->m_pSaveDelegate = delegate;


	this->send(network::HttpRequest::Type::GET);
}

void BmobUser::loginByAccount(string mebileNumber,string pwd,BmobLoginDelegate* delegate){
	if (mebileNumber.empty() || pwd.empty())
	{
		/* code */
		return ;
	}

	this->m_url = BmobSDKInit::LOGIN_URL  + "?username=" + \
					mebileNumber + "&password=" + pwd;
	this->clear();

	/**
	* save user
	*/
	UserDefault::getInstance()->setStringForKey("user_name",mebileNumber);
	UserDefault::getInstance()->setStringForKey("user_pwd",pwd);

	_opType = HTTP_OP_Type::_bHTTP_LOGIN;

	this->m_pLoginDelegate = delegate;


	this->send(network::HttpRequest::Type::GET);

}

void BmobUser::loginBySMSCode(string mebileNumber,string code,BmobLoginDelegate* delegate){
	if (mebileNumber.empty() || code.empty())
	{
		/* code */
		return ;
	}

	this->m_url = BmobSDKInit::LOGIN_URL  + "?mobilePhoneNumber=" + \
					mebileNumber + "&smsCode=" + code;
	this->clear();

	/**
	* save user
	*/
	// UserDefault::getInstance()->setStringForKey("user_name",mebileNumber);
	// UserDefault::getInstance()->setStringForKey("user_pwd",pwd);

	_opType = HTTP_OP_Type::_bHTTP_LOGIN;

	this->m_pLoginDelegate = delegate;


	this->send(network::HttpRequest::Type::GET);

}

void BmobUser::signOrLoginByMobilePhone(string mebileNumber,string code,BmobLoginDelegate* delegate){
	this->loginBySMSCode(mebileNumber,code,delegate);
}

void BmobUser::update(string objectId,BmobUpdateDelegate* delegate){
	if (objectId.empty())
    {
        /* code */
        return ;
    }
     _opType = HTTP_OP_Type::_bHTTP_UPDATE;
    this->m_pUpdateDelegate = delegate;

    this->m_url =  BmobSDKInit::USER_URL;

    string session ;
	if (m_session.empty())
	{
		/* code */
		session = UserDefault::getInstance()->getStringForKey("user_session");
	}else{
		session = m_session;
	}

    this->setSession(session);

    if (!objectId.empty())
    {
        /* code */
        this->m_url += + "/" + objectId;
    }

    this->send(network::HttpRequest::Type::PUT);
}

void BmobUser::update(BmobUpdateDelegate* delegate){
	this->update(this->getObjectId(),delegate);
}

void BmobUser::resetPasswordByEmail(string email,BmobResetPasswordDelegate* delegate){
	if (email.empty())
	{
		/* code */
		return ;
	}

	_opType = HTTP_OP_Type::_bHTTP_RESET;
	this->m_pResetDelegate = delegate;
	this->m_url = BmobSDKInit::RESET_URL + "?";

	this->clear();

	this->enParamsToHttp("email",__String::createWithFormat("%s",email.c_str()));

	this->send();
}

void BmobUser::requestSMSCode(string meblieNumber,string template_name,BmobRequestSMSCodeDelegate* delegate){
	if (meblieNumber.empty())
	{
		/* code */
		return ;
	}

	_opType = HTTP_OP_Type::_bHTTP_REQUEST_CODE;
	this->m_pRequestSMSCodeDelegate = delegate;
	this->m_url = BmobSDKInit::REQUEST_SMS_CODE_URL + "?";

	this->clear();

	this->enParamsToHttp("mobilePhoneNumber",__String::createWithFormat("%s",meblieNumber.c_str()));
	if (!template_name.empty())
	{
		/*code */
		this->enParamsToHttp("template",__String::createWithFormat("%s",template_name.c_str()));
	}

	this->send();
}

void BmobUser::resetPasswordBySMSCode(string pw,string code,BmobResetPasswordByCodeDelegate* delegate){
	if (pw.empty() || code.empty())
	{
		/* code */
		return ;
	}

	_opType = HTTP_OP_Type::_bHTTP_RESET_BY_CODE;
	this->m_pResetByMSMCodeDelegate = delegate;
	this->m_url = BmobSDKInit::RESET_BY_CODE_URL + "/" + code;

	this->clear();

	UserDefault::getInstance()->setStringForKey("user_pwd",pw);

	this->enParamsToHttp("password",__String::createWithFormat("%s",pw.c_str()));

	this->send(network::HttpRequest::Type::PUT);
}

void BmobUser::updateCurrentUserPassword(string old_pwd,string new_pwd,BmobUpdateDelegate* delegate){
	if (old_pwd.empty() || new_pwd.empty() || m_objectId.empty())
	{
		/* code */
		return ;
	}

	this->m_pUpdateDelegate = delegate;
	_opType = HTTP_OP_Type::_bHTTP_UPDATE;

	string session ;
	if (m_session.empty())
	{
		/* code */
		UserDefault::getInstance()->getStringForKey("user_session");
	}else{
		session = m_session;
	}
	this->setSession(session);

	this->clear();

	this->m_url = BmobSDKInit::UPDATE_PWD_URL + "/" + m_objectId;

	UserDefault::getInstance()->setStringForKey("user_pwd",new_pwd);

	this->enParamsToHttp("oldPassword",__String::createWithFormat("%s",old_pwd.c_str()));
	this->enParamsToHttp("newPassword",__String::createWithFormat("%s",new_pwd.c_str()));

	this->send(network::HttpRequest::Type::PUT);
}

void BmobUser::requestEmailVerify(string email,BmobEmailVerifyDelegate* delegate){
	if (email.empty())
	{
		/* code */
		return  ;
	}

	this->m_pEmailVerifyDelegate = delegate;
	_opType = HTTP_OP_Type::_bHTTP_EMAIL_VERIFY;

	this->m_url = BmobSDKInit::EMAIL_VERIFY_URL;

	this->clear();

	this->enParamsToHttp("email",__String::createWithFormat("%s",email.c_str()));

	this->send();
}
