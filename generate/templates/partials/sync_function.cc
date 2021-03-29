<%_ for (const _return of func.outParams) { _%>
    <% if (_return.shouldAlloc) { _%>
    <%- _return.rawType %> *<%- _return.name %> = new <%- _return.rawType %>;
    <% } else { _%>
    <%- _return.rawType %> <%- _return.isPointer ? '*' : '' %> <%- _return.name %> = NULL;
    <%_ } _%>
<%_ } _%>

<%_ for (const arg of func.params) { _%>
    <%_ if (!arg.isReturn) { _%>
        <%- include('convert_from_js.cc', { arg: arg }) %>
    <%_ } _%>
<%_ } _%>


<% if (func.returnType.isReturn || func.returnType.isErrorCode) { _%> <%- func.returnType.const %> <%- func.returnType.rawType %> <%- func.returnType.ref %> <%- func.returnType.name %> = <% } _%>
<%_ if (!func.isStatic) { _%>_underlying-><% } else { _%>::<%_ } _%><%- func.name %>(
<%_ for (const arg of func.params) { _%>
    <% if (arg.isCppString2CString) { _%>
    <%- arg.name %>.c_str(), <%- arg.name %>.length()
    <%_ } else if (arg.shouldAlloc) { _%>
    *<%- arg.name %>
    <%_ } else if (arg.isOptional) { _%>
    info.Length() == <%- arg.cppIndex %> || info[<%- arg.cppIndex %>].IsNull() ? <%- arg.default %> : <%- arg.name %>
    <%_ } else { _%>
    <%- arg.name %>
    <%_ } _%>
    <%_ if (arg.cppIndex < func.params.length - 1) { _%>,<%_ } _%>
<%_ } _%>
);


<%_ if (func.returnType.isErrorCode) { _%>
if (_result == rt_Success) {
<%_ } _%>

<%_ if (func.returnsCount == 0) { _%>
    return env.Undefined();
<%_ } else { _%>
    <%_ if (func.returnsCount > 1) { _%>
        Napi::Object _toReturn = Napi::Object::New(env);
    <%_ } _%>
    Napi::Value _to;

    <%_ for (_return of func.returns) { _%>
        <%- include('convert_to_js.cc', { arg: _return }) %>
        <%_ if (func.returnsCount > 1) { _%>
        _toReturn.Set(Napi::String::New(env, "<%- _return.name %>"), _to);
        <%_ } _%>
    <%_ } _%>
    <% if (func.returnsCount == 1) { _%>
        return _to;
    <%_ } else { _%>
        return _toReturn;
    <%_ } _%>
<%_ } _%>

<% if (func.returnType.isErrorCode) { _%>
} else {
    std::ostringstream msg;
    msg << "Operation <%- func.name %> failed with error: " << Error::GetSolidErrorResId(_result);
    Napi::Error::New(env, msg.str()).ThrowAsJavaScriptException();
    return env.Undefined();
}
<%_ } _%>