using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Text;

namespace LittleQuant.Commons.HttpClient
{
    public interface IHttpClient
    {
        /// <summary>
        /// 发送get请求
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="uri">资源uri</param>
        /// <param name="headers">http头</param>
        /// <param name="specialHeaders">特殊http头，如referrer,accept等</param>
        /// <param name="deserializer">响应内容的反序列化器，仅在T为String时才可传null</param>
        /// <param name="enc">响应内容的编码</param>
        /// <returns></returns>
        T Get<T>(string uri,
                 NameValueCollection headers = null,
                 SpecialHeaders specialHeaders = null,
                 Func<String, T> deserializer = null,
                 Encoding enc = null);

        /// <summary>
        /// 发送post请求
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="uri">资源uri</param>
        /// <param name="form">post的数据 { "key1", "value1", "key2", "value2" }</param>
        /// <param name="headers">http头</param>
        /// <param name="specialHeaders">特殊http头，如referrer,accept等</param>
        /// <param name="deserializer">响应内容的反序列化器，仅在T为String时才可传null</param>
        /// <param name="enc">响应内容的编码</param>
        /// <returns></returns>
        T Post<T>(string uri,
                  IList<string> form,
                  NameValueCollection headers = null,
                  SpecialHeaders specialHeaders = null,
                  Func<String, T> deserializer = null,
                  Encoding enc = null);

        string GetCookie(string name);
    }
}
