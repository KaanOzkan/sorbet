#include "main/lsp/tools/make_lsp_types.h"

std::shared_ptr<JSONType> makeStrEnum(const std::string name, vector<const std::string> values,
                                      std::vector<std::shared_ptr<JSONClassType>> &enumTypes) {
    std::shared_ptr<JSONStringEnumType> ct = std::make_shared<JSONStringEnumType>(name, values);
    enumTypes.push_back(ct);
    return ct;
}

std::shared_ptr<JSONType> makeIntEnum(const std::string name, vector<std::pair<const std::string, int>> values,
                                      std::vector<std::shared_ptr<JSONClassType>> &enumTypes) {
    auto et = std::make_shared<JSONIntEnumType>(name, values);
    enumTypes.push_back(et);
    return et;
}

std::shared_ptr<JSONType> makeStringConstant(const std::string name) {
    return std::make_shared<JSONStringConstantType>(name);
}

std::shared_ptr<FieldDef> makeField(const std::string name, std::shared_ptr<JSONType> type) {
    return std::make_shared<FieldDef>(name, type);
}

std::shared_ptr<JSONObjectType> makeObject(const std::string name, vector<std::shared_ptr<FieldDef>> fields,
                                           std::vector<std::shared_ptr<JSONObjectType>> &classTypes) {
    std::shared_ptr<JSONObjectType> ct = std::make_shared<JSONObjectType>(name, fields);
    classTypes.push_back(ct);
    return ct;
}

std::shared_ptr<JSONType> makeOptional(std::shared_ptr<JSONType> type) {
    return std::make_shared<JSONOptionalType>(type);
}

std::shared_ptr<JSONType> makeVariant(std::vector<std::shared_ptr<JSONType>> variants) {
    return std::make_shared<JSONVariantType>(variants);
}

std::shared_ptr<JSONType> makeArray(std::shared_ptr<JSONType> type) {
    return std::make_shared<JSONArrayType>(type);
}

void makeLSPTypes(std::vector<std::shared_ptr<JSONClassType>> &enumTypes,
                  std::vector<std::shared_ptr<JSONObjectType>> &classTypes) {
    // Singletons
    std::shared_ptr<JSONType> JSONAny = std::make_shared<JSONAnyType>();
    std::shared_ptr<JSONType> JSONAnyObject = std::make_shared<JSONAnyObjectType>();
    std::shared_ptr<JSONType> JSONNull = std::make_shared<JSONNullType>();
    std::shared_ptr<JSONType> JSONBool = std::make_shared<JSONBooleanType>();
    std::shared_ptr<JSONType> JSONInt = std::make_shared<JSONIntType>();
    std::shared_ptr<JSONType> JSONDouble = std::make_shared<JSONDoubleType>();
    std::shared_ptr<JSONType> JSONString = std::make_shared<JSONStringType>();

    // Converted from https://microsoft.github.io/language-server-protocol/specification
    // Last updated on 11/14/18.

    auto ResourceOperationKind = makeStrEnum("ResourceOperationKind", {"create", "rename", "delete"}, enumTypes);
    auto MarkupKind = makeStrEnum("MarkupKind", {"plaintext", "markdown"}, enumTypes);
    auto TraceKind = makeStrEnum("TraceKind", {"off", "messages", "verbose"}, enumTypes);
    auto JSONRPCConstant = makeStringConstant("2.0");
    auto CreateConstant = makeStringConstant("create");
    auto RenameConstant = makeStringConstant("rename");
    auto DeleteConstant = makeStringConstant("delete");

    auto RequestMessage =
        makeObject("RequestMessage",
                   {
                       makeField("jsonrpc", JSONRPCConstant),
                       makeField("id", makeVariant({JSONInt, JSONString})),
                       makeField("method", JSONString),
                       makeField("params", makeOptional(makeVariant({makeArray(JSONAny), JSONAnyObject}))),
                   },
                   classTypes);
    auto ResponseError = makeObject("ResponseError",
                                    {
                                        makeField("code", JSONInt),
                                        makeField("message", JSONString),
                                        makeField("data", makeOptional(JSONAny)),
                                    },
                                    classTypes);
    auto ResponseMessage = makeObject("ResponseMessage",
                                      {
                                          makeField("jsonrpc", JSONRPCConstant),
                                          makeField("id", makeVariant({JSONInt, JSONString, JSONNull})),
                                          makeField("result", makeOptional(JSONAny)),
                                          makeField("error", makeOptional(ResponseError)),
                                      },
                                      classTypes);
    auto NotificationMessage =
        makeObject("NotificationMessage",
                   {
                       makeField("jsonrpc", JSONRPCConstant),
                       makeField("method", JSONString),
                       makeField("params", makeOptional(makeVariant({makeArray(JSONAny), JSONAnyObject}))),
                   },
                   classTypes);

    auto CancelParams = makeObject("CancelParams",
                                   {
                                       makeField("id", makeVariant({JSONInt, JSONString})),
                                   },
                                   classTypes);

    auto Position = makeObject("Position",
                               {
                                   makeField("line", JSONInt),
                                   makeField("character", JSONInt),
                               },
                               classTypes);

    auto Range = makeObject("Range",
                            {
                                makeField("start", Position),
                                makeField("end", Position),
                            },
                            classTypes);

    auto Location = makeObject("Location",
                               {
                                   makeField("uri", JSONString),
                                   makeField("range", Range),
                               },
                               classTypes);

    auto DiagnosticRelatedInformation = makeObject("DiagnosticRelatedInformation",
                                                   {
                                                       makeField("location", Location),
                                                       makeField("message", JSONString),
                                                   },
                                                   classTypes);

    auto DiagnosticSeverity =
        makeIntEnum("DiagnosticSeverity", {{"Error", 1}, {"Warning", 2}, {"Information", 3}, {"Hint", 4}}, enumTypes);

    auto Diagnostic =
        makeObject("Diagnostic",
                   {
                       makeField("range", Range),
                       makeField("severity", makeOptional(DiagnosticSeverity)),
                       makeField("code", makeOptional(makeVariant({JSONDouble, JSONString}))),
                       makeField("source", makeOptional(JSONString)),
                       makeField("message", JSONString),
                       makeField("relatedInformation", makeOptional(makeArray(DiagnosticRelatedInformation))),
                   },
                   classTypes);

    auto Command = makeObject("Command",
                              {
                                  makeField("title", JSONString),
                                  makeField("command", JSONString),
                                  makeField("arguments", makeOptional(makeArray(JSONAny))),
                              },
                              classTypes);

    auto TextEdit = makeObject("TextEdit",
                               {
                                   makeField("range", Range),
                                   makeField("newText", JSONString),
                               },
                               classTypes);

    auto VersionedTextDocumentIdentifier = makeObject("VersionedTextDocumentIdentifier",
                                                      {
                                                          makeField("uri", JSONString),
                                                          makeField("version", makeVariant({JSONDouble, JSONNull})),
                                                      },
                                                      classTypes);

    auto TextDocumentEdit = makeObject("TextDocumentEdit",
                                       {
                                           makeField("textDocument", VersionedTextDocumentIdentifier),
                                           makeField("edits", makeArray(TextEdit)),
                                       },
                                       classTypes);

    // These have the same shape.
    auto CreateOrRenameFileOptions = makeObject("CreateOrRenameFileOptions",
                                                {
                                                    makeField("overwrite", makeOptional(JSONBool)),
                                                    makeField("ignoreIfExists", makeOptional(JSONBool)),
                                                },
                                                classTypes);

    auto CreateFile = makeObject("CreateFile",
                                 {
                                     makeField("kind", CreateConstant),
                                     makeField("uri", JSONString),
                                     makeField("options", makeOptional(CreateOrRenameFileOptions)),
                                 },
                                 classTypes);
    auto RenameFile = makeObject("RenameFile",
                                 {
                                     makeField("kind", RenameConstant),
                                     makeField("oldUri", JSONString),
                                     makeField("newUri", JSONString),
                                     makeField("options", makeOptional(CreateOrRenameFileOptions)),
                                 },
                                 classTypes);

    auto DeleteFileOptions = makeObject("DeleteFileOptions",
                                        {
                                            makeField("recursive", makeOptional(JSONBool)),
                                            makeField("ignoreIfNotExists", makeOptional(JSONBool)),
                                        },
                                        classTypes);

    auto DeleteFile = makeObject("DeleteFile",
                                 {
                                     makeField("kind", DeleteConstant),
                                     makeField("uri", JSONString),
                                     makeField("options", makeOptional(DeleteFileOptions)),
                                 },
                                 classTypes);

    auto WorkspaceEdit = makeObject("WorkspaceEdit",
                                    {
                                        // TODO(jvilk): Map type.
                                        makeField("changes", makeArray(JSONAny)),
                                        // TODO(jvilk): Variant that discriminates on string constant + missing fields.
                                        makeField("documentChanges", makeOptional(makeArray(JSONAny))),
                                    },
                                    classTypes);

    auto TextDocumentIdentifier = makeObject("TextDocumentIdentifier",
                                             {
                                                 makeField("uri", JSONString),
                                             },
                                             classTypes);

    auto TextDocumentItem = makeObject("TextDocumentItem",
                                       {
                                           makeField("uri", JSONString),
                                           makeField("languageId", JSONString),
                                           makeField("version", JSONInt),
                                           makeField("text", JSONString),
                                       },
                                       classTypes);

    auto TextDocumentPositionParams = makeObject("TextDocumentPositionParams",
                                                 {
                                                     makeField("textDocument", TextDocumentIdentifier),
                                                     makeField("position", Position),
                                                 },
                                                 classTypes);

    auto DocumentFilter = makeObject("DocumentFilter",
                                     {
                                         makeField("language", makeOptional(JSONString)),
                                         makeField("scheme", makeOptional(JSONString)),
                                         makeField("pattern", makeOptional(JSONString)),
                                     },
                                     classTypes);

    auto MarkupContent = makeObject("MarkupContent",
                                    {
                                        makeField("kind", MarkupKind),
                                        makeField("value", JSONString),
                                    },
                                    classTypes);

    auto FailureHandlingKind =
        makeStrEnum("FailureHandlingKind", {"abort", "transactional", "undo", "textOnlyTransactional"}, enumTypes);

    auto WorkspaceEditCapabilities =
        makeObject("WorkspaceEditCapabilities",
                   {
                       makeField("documentChanges", makeOptional(JSONBool)),
                       makeField("resourceOperations", makeOptional(makeArray(ResourceOperationKind))),
                       makeField("failureHandling", makeOptional(FailureHandlingKind)),
                   },
                   classTypes);

    auto DynamicRegistrationOption = makeObject("DynamicRegistrationOption",
                                                {
                                                    makeField("dynamicRegistration", makeOptional(JSONBool)),
                                                },
                                                classTypes);

    auto SymbolKind = makeIntEnum("SymbolKind",
                                  {
                                      {"File", 1},        {"Module", 2},         {"Namespace", 3},  {"Package", 4},
                                      {"Class", 5},       {"Method", 6},         {"Property", 7},   {"Field", 8},
                                      {"Constructor", 9}, {"Enum", 10},          {"Interface", 11}, {"Function", 12},
                                      {"Variable", 13},   {"Constant", 14},      {"String", 15},    {"Number", 16},
                                      {"Boolean", 17},    {"Array", 18},         {"Object", 19},    {"Key", 20},
                                      {"Null", 21},       {"EnumMember", 22},    {"Struct", 23},    {"Event", 24},
                                      {"Operator", 25},   {"TypeParameter", 26},
                                  },
                                  enumTypes);

    auto SymbolKindOption = makeObject("SymbolKindOptions",
                                       {
                                           makeField("valueSet", makeOptional(makeArray(SymbolKind))),
                                       },
                                       classTypes);

    auto SymbolConfiguration = makeObject("SymbolConfiguration",
                                          {
                                              makeField("dynamicRegistration", makeOptional(JSONBool)),
                                              makeField("symbolKind", makeOptional(SymbolKindOption)),
                                          },
                                          classTypes);

    auto WorkspaceClientCapabilities =
        makeObject("WorkspaceClientCapabilities",
                   {
                       makeField("applyEdit", makeOptional(JSONBool)),
                       makeField("workspaceEdit", makeOptional(WorkspaceEditCapabilities)),
                       makeField("didChangeConfiguration", makeOptional(DynamicRegistrationOption)),
                       makeField("didChangeWatchedFiles", makeOptional(DynamicRegistrationOption)),
                       makeField("symbol", makeOptional(SymbolConfiguration)),
                       makeField("executeCommand", makeOptional(DynamicRegistrationOption)),
                       makeField("workspaceFolders", makeOptional(JSONBool)),
                       makeField("configuration", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto SynchronizationCapabilities = makeObject("SynchronizationCapabilities",
                                                  {
                                                      makeField("dynamicRegistration", makeOptional(JSONBool)),
                                                      makeField("willSave", makeOptional(JSONBool)),
                                                      makeField("willSaveWaitUntil", makeOptional(JSONBool)),
                                                      makeField("didSave", makeOptional(JSONBool)),
                                                  },
                                                  classTypes);

    auto CompletionItemCapabilities =
        makeObject("CompletionItemCapabilities",
                   {
                       makeField("snippetSupport", makeOptional(JSONBool)),
                       makeField("commitCharactersSupport", makeOptional(JSONBool)),
                       makeField("documentationFormat", makeOptional(makeArray(MarkupKind))),
                       makeField("deprecatedSupport", makeOptional(JSONBool)),
                       makeField("preselectSupport", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto CompletionItemKind =
        makeIntEnum("CompletionItemKind",
                    {
                        {"Text", 1},      {"Method", 2},  {"Function", 3},   {"Constructor", 4}, {"Field", 5},
                        {"Variable", 6},  {"Class", 7},   {"Interface", 8},  {"Module", 9},      {"Property", 10},
                        {"Unit", 11},     {"Value", 12},  {"Enum", 13},      {"Keyword", 14},    {"Snippet", 15},
                        {"Color", 16},    {"File", 17},   {"Reference", 18}, {"Folder", 19},     {"EnumMember", 20},
                        {"Constant", 21}, {"Struct", 22}, {"Event", 23},     {"Operator", 24},   {"TypeParameter", 25},
                    },
                    enumTypes);

    auto CompletionItemKindCapabilities =
        makeObject("CompletionItemKindCapabilities",
                   {
                       makeField("valueSet", makeOptional(makeArray(CompletionItemKind))),
                   },
                   classTypes);

    auto CompletionCapabilities =
        makeObject("CompletionCapabilities",
                   {
                       makeField("dynamicRegistration", makeOptional(JSONBool)),
                       makeField("completionItem", makeOptional(CompletionItemCapabilities)),
                       makeField("completionItemKind", makeOptional(CompletionItemKindCapabilities)),
                       makeField("contextSupport", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto HoverCapabilities = makeObject("HoverCapabilities",
                                        {
                                            makeField("dynamicRegistration", makeOptional(JSONBool)),
                                            makeField("contentFormat", makeOptional(makeArray(MarkupKind))),
                                        },
                                        classTypes);

    auto SignatureInformationCapabilities =
        makeObject("SignatureInformationCapabilities",
                   {
                       makeField("documentationFormat", makeOptional(makeArray(MarkupKind))),
                   },
                   classTypes);

    auto SignatureHelpCapabilities =
        makeObject("SignatureHelpCapabilities",
                   {
                       makeField("dynamicRegistration", makeOptional(JSONBool)),
                       makeField("signatureInformation", makeOptional(SignatureInformationCapabilities)),
                   },
                   classTypes);

    auto DocumentSymbolCapabilities =
        makeObject("DocumentSymbolCapabilities",
                   {
                       makeField("dynamicRegistration", makeOptional(JSONBool)),
                       makeField("symbolKind", makeOptional(SymbolKindOption)),
                       makeField("hierarchicalDocumentSymbolSupport", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto CodeActionKind = makeStrEnum("CodeActionKind",
                                      {"quickfix", "refactor", "refactor.extract", "refactor.inline",
                                       "refactor.rewrite", "source", "source.organizeImports"},
                                      enumTypes);

    auto CodeActionKindSupport = makeObject("CodeActionKindSupport",
                                            {
                                                makeField("valueSet", makeArray(CodeActionKind)),
                                            },
                                            classTypes);

    auto CodeActionLiteralSupport = makeObject("CodeActionLiteralSupport",
                                               {
                                                   makeField("codeActionKind", CodeActionKindSupport),
                                               },
                                               classTypes);

    auto CodeActionCapabilities =
        makeObject("CodeActionCapabilities",
                   {
                       makeField("dynamicRegistration", makeOptional(JSONBool)),
                       makeField("codeActionLiteralSupport", makeOptional(CodeActionLiteralSupport)),
                   },
                   classTypes);

    auto RenameCapabilities = makeObject("RenameCapabilities",
                                         {
                                             makeField("dynamicRegistration", makeOptional(JSONBool)),
                                             makeField("prepareSupport", makeOptional(JSONBool)),
                                         },
                                         classTypes);

    auto PublishDiagnosticsCapabilities = makeObject("PublishDiagnosticsCapabilities",
                                                     {
                                                         makeField("relatedInformation", makeOptional(JSONBool)),
                                                     },
                                                     classTypes);

    auto FoldingRangeCapabilities = makeObject("FoldingRangeCapabilities",
                                               {
                                                   makeField("dynamicRegistration", makeOptional(JSONBool)),
                                                   makeField("rangeLimit", makeOptional(JSONInt)),
                                                   makeField("lineFoldingOnly", makeOptional(JSONBool)),
                                               },
                                               classTypes);

    auto TextDocumentClientCapabilities =
        makeObject("TextDocumentClientCapabilities",
                   {
                       makeField("synchronization", makeOptional(SynchronizationCapabilities)),
                       makeField("completion", makeOptional(CompletionCapabilities)),
                       makeField("hover", makeOptional(HoverCapabilities)),
                       makeField("signatureHelp", makeOptional(SignatureHelpCapabilities)),
                       makeField("references", makeOptional(DynamicRegistrationOption)),
                       makeField("documentHighlight", makeOptional(DynamicRegistrationOption)),
                       makeField("documentSymbol", makeOptional(DocumentSymbolCapabilities)),
                       makeField("formatting", makeOptional(DynamicRegistrationOption)),
                       makeField("rangeFormatting", makeOptional(DynamicRegistrationOption)),
                       makeField("onTypeFormatting", makeOptional(DynamicRegistrationOption)),
                       makeField("definition", makeOptional(DynamicRegistrationOption)),
                       makeField("typeDefinition", makeOptional(DynamicRegistrationOption)),
                       makeField("implementation", makeOptional(DynamicRegistrationOption)),
                       makeField("codeAction", makeOptional(CodeActionCapabilities)),
                       makeField("codeLens", makeOptional(DynamicRegistrationOption)),
                       makeField("documentLink", makeOptional(DynamicRegistrationOption)),
                       makeField("colorProvider", makeOptional(DynamicRegistrationOption)),
                       makeField("rename", makeOptional(RenameCapabilities)),
                       makeField("publishDiagnostics", makeOptional(PublishDiagnosticsCapabilities)),
                       makeField("foldingRange", makeOptional(FoldingRangeCapabilities)),
                   },
                   classTypes);

    auto ClientCapabilities = makeObject("ClientCapabilities",
                                         {
                                             makeField("workspace", makeOptional(WorkspaceClientCapabilities)),
                                             makeField("textDocument", makeOptional(TextDocumentClientCapabilities)),
                                             makeField("experimental", makeOptional(JSONAny)),
                                         },
                                         classTypes);

    auto CompletionOptions = makeObject("CompletionOptions",
                                        {
                                            makeField("resolveProvider", makeOptional(JSONBool)),
                                            makeField("triggerCharacters", makeOptional(makeArray(JSONString))),
                                        },
                                        classTypes);

    auto SignatureHelpOptions = makeObject(
        "SignatureHelpOptions", {makeField("triggerCharacters", makeOptional(makeArray(JSONString)))}, classTypes);

    auto CodeActionOptions = makeObject("CodeActionOptions",
                                        {
                                            makeField("codeActionKinds", makeOptional(makeArray(CodeActionKind))),
                                        },
                                        classTypes);

    auto CodeLensOptions = makeObject("CodeLensOptions",
                                      {
                                          makeField("resolveProvider", makeOptional(JSONBool)),
                                      },
                                      classTypes);

    auto DocumentOnTypeFormattingOptions =
        makeObject("DocumentOnTypeFormattingOptions",
                   {
                       makeField("firstTriggerCharacter", JSONString),
                       makeField("moreTriggerCharacter", makeOptional(makeArray(JSONString))),
                   },
                   classTypes);

    auto RenameOptions = makeObject("RenameOptions",
                                    {
                                        makeField("prepareProvider", JSONBool),
                                    },
                                    classTypes);

    auto DocumentLinkOptions = makeObject("DocumentLinkOptions",
                                          {
                                              makeField("resolveProvider", JSONBool),
                                          },
                                          classTypes);

    auto ExecuteCommandOptions = makeObject("ExecuteCommandOptions",
                                            {
                                                makeField("commands", makeArray(JSONString)),
                                            },
                                            classTypes);

    auto SaveOptions = makeObject("SaveOptions",
                                  {
                                      makeField("includeText", makeOptional(JSONBool)),
                                  },
                                  classTypes);

    auto EmptyOptions = makeObject("EmptyOptions", {}, classTypes);

    auto TextDocumentSyncKind = makeIntEnum("TextDocumentSyncKind",
                                            {
                                                {"None", 0},
                                                {"Full", 1},
                                                {"Incremental", 2},
                                            },
                                            enumTypes);

    auto TextDocumentSyncOptions = makeObject("TextDocumentSyncOptions",
                                              {
                                                  makeField("openClose", makeOptional(JSONBool)),
                                                  makeField("change", makeOptional(TextDocumentSyncKind)),
                                                  makeField("willSave", makeOptional(JSONBool)),
                                                  makeField("willSaveWaitUntil", makeOptional(JSONBool)),
                                                  makeField("save", makeOptional(SaveOptions)),
                                              },
                                              classTypes);

    auto DocumentSelector = makeArray(DocumentFilter);

    auto TextDocumentAndStaticRegistrationOptions =
        makeObject("TextDocumentAndStaticRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({JSONNull, DocumentSelector})),
                       makeField("id", JSONString),
                   },
                   classTypes);

    auto WorkspaceFoldersOptions =
        makeObject("WorkspaceFoldersOptions",
                   {
                       makeField("supported", makeOptional(JSONBool)),
                       makeField("changeNotifications", makeOptional(makeVariant({JSONString, JSONBool}))),
                   },
                   classTypes);

    auto WorkspaceOptions = makeObject("WorkspaceOptions",
                                       {
                                           makeField("workspaceFolders", makeOptional(WorkspaceFoldersOptions)),
                                       },
                                       classTypes);

    auto ServerCapabilities = makeObject(
        "ServerCapabilities",
        {
            makeField("textDocumentSync", makeOptional(makeVariant({TextDocumentSyncOptions, TextDocumentSyncKind}))),
            makeField("hoverProvider", makeOptional(JSONBool)),
            makeField("completionProvider", makeOptional(CompletionOptions)),
            makeField("signatureHelpProvider", makeOptional(SignatureHelpOptions)),
            makeField("definitionProvider", makeOptional(JSONBool)),
            makeField("typeDefinitionProvider",
                      makeOptional(makeVariant({JSONBool, TextDocumentAndStaticRegistrationOptions}))),
            makeField("implementationProvider",
                      makeOptional(makeVariant({JSONBool, TextDocumentAndStaticRegistrationOptions}))),
            makeField("referencesProvider", makeOptional(JSONBool)),
            makeField("documentHighlightProvider", makeOptional(JSONBool)),
            makeField("documentSymbolProvider", makeOptional(JSONBool)),
            makeField("workspaceSymbolProvider", makeOptional(JSONBool)),
            makeField("codeActionProvider", makeOptional(makeVariant({JSONBool, CodeActionOptions}))),
            makeField("codeLensProvider", makeOptional(CodeLensOptions)),
            makeField("documentFormattingProvider", makeOptional(JSONBool)),
            makeField("documentRangeFormattingProvider", makeOptional(JSONBool)),
            makeField("documentOnTypeFormattingProvider", makeOptional(DocumentOnTypeFormattingOptions)),
            makeField("renameProvider", makeOptional(makeVariant({JSONBool, RenameOptions}))),
            makeField("documentLinkProvider", makeOptional(DocumentLinkOptions)),
            makeField("colorProvider", makeOptional(makeVariant({JSONBool, JSONAnyObject}))),
            makeField("foldingRangeProvider", makeOptional(makeVariant({JSONBool, JSONAnyObject}))),
            makeField("executeCommandProvider", makeOptional(ExecuteCommandOptions)),
            makeField("workspace", makeOptional(WorkspaceOptions)),
            makeField("experimental", makeOptional(JSONAny)),
        },
        classTypes);

    auto MessageType = makeIntEnum("MessageType",
                                   {
                                       {"Error", 1},
                                       {"Warning", 2},
                                       {"Info", 3},
                                       {"Log", 4},
                                   },
                                   enumTypes);

    auto ShowMessageParams = makeObject("ShowMessageParams",
                                        {
                                            makeField("type", MessageType),
                                            makeField("message", JSONString),
                                        },
                                        classTypes);

    auto MessageActionItem = makeObject("MessageActionItem",
                                        {
                                            makeField("title", JSONString),
                                        },
                                        classTypes);

    auto ShowMessageRequestParams = makeObject("ShowMessageRequestParams",
                                               {
                                                   makeField("type", MessageType),
                                                   makeField("message", JSONString),
                                                   makeField("actions", makeOptional(makeArray(MessageActionItem))),
                                               },
                                               classTypes);

    auto LogMessageParams = makeObject("LogMessageParams",
                                       {
                                           makeField("type", MessageType),
                                           makeField("message", JSONString),
                                       },
                                       classTypes);

    auto Registration = makeObject("Registration",
                                   {
                                       makeField("id", JSONString),
                                       makeField("method", JSONString),
                                       makeField("registerOptions", makeOptional(JSONAny)),
                                   },
                                   classTypes);

    auto RegistrationParams = makeObject("RegistrationParams",
                                         {
                                             makeField("registrations", makeArray(Registration)),
                                         },
                                         classTypes);

    auto TextDocumentRegistrationOptions =
        makeObject("TextDocumentRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                   },
                   classTypes);

    auto Unregistration = makeObject("Unregistration",
                                     {
                                         makeField("id", JSONInt),
                                         makeField("method", JSONString),
                                     },
                                     classTypes);

    auto UnregistrationParams = makeObject("UnregistrationParams",
                                           {
                                               makeField("unregistrations", makeArray(Unregistration)),
                                           },
                                           classTypes);

    auto WorkspaceFolder = makeObject("WorkspaceFolder",
                                      {
                                          makeField("uri", JSONString),
                                          makeField("name", JSONString),
                                      },
                                      classTypes);

    auto WorkspaceFoldersChangeEvent = makeObject("WorkspaceFoldersChangeEvent",
                                                  {
                                                      makeField("added", makeArray(WorkspaceFolder)),
                                                      makeField("removed", makeArray(WorkspaceFolder)),
                                                  },
                                                  classTypes);

    auto DidChangeWorkspaceFoldersParams = makeObject("DidChangeWorkspaceFoldersParams",
                                                      {
                                                          makeField("event", WorkspaceFoldersChangeEvent),
                                                      },
                                                      classTypes);

    auto DidChangeConfigurationParams = makeObject("DidChangeConfigurationParams",
                                                   {
                                                       makeField("settings", JSONAny),
                                                   },
                                                   classTypes);

    auto ConfigurationItem = makeObject("ConfigurationItem",
                                        {
                                            makeField("scopeUri", makeOptional(JSONString)),
                                            makeField("section", makeOptional(JSONString)),
                                        },
                                        classTypes);

    auto ConfigurationParams = makeObject("ConfigurationParams",
                                          {
                                              makeField("items", makeArray(ConfigurationItem)),
                                          },
                                          classTypes);

    auto FileChangeType = makeIntEnum("FileChangeType",
                                      {
                                          {"Created", 1},
                                          {"Changed", 2},
                                          {"Deleted", 3},
                                      },
                                      enumTypes);

    auto FileEvent = makeObject("FileEvent",
                                {
                                    makeField("uri", JSONString),
                                    makeField("type", FileChangeType),
                                },
                                classTypes);

    auto DidChangeWatchedFilesParams = makeObject("DidChangeWatchedFilesParams",
                                                  {
                                                      makeField("changes", makeArray(FileEvent)),
                                                  },
                                                  classTypes);

    // Note: Three base enum values are used as bitmasks, so I filled in the gaps.
    auto WatchKind = makeIntEnum("WatchKind",
                                 {{"Create", 1},
                                  {"Change", 2},
                                  {"CreateAndChange", 3},
                                  {"Delete", 4},
                                  {"CreateAndDelete", 5},
                                  {"ChangeAndDelete", 6},
                                  {"CreateAndChangeAndDelete", 7}},
                                 enumTypes);

    auto FileSystemWatcher = makeObject("FileSystemWatcher",
                                        {
                                            makeField("globPattern", JSONString),
                                            makeField("kind", makeOptional(WatchKind)),
                                        },
                                        classTypes);

    auto DidChangeWatchedFilesRegistrationOptions = makeObject("DidChangeWatchedFilesRegistrationOptions",
                                                               {
                                                                   makeField("watchers", makeArray(FileSystemWatcher)),
                                                               },
                                                               classTypes);

    auto WorkspaceSymbolParams = makeObject("WorkspaceSymbolParams",
                                            {
                                                makeField("query", JSONString),
                                            },
                                            classTypes);

    auto ExecuteCommandParams = makeObject("ExecuteCommandParams",
                                           {
                                               makeField("command", JSONString),
                                               makeField("arguments", makeOptional(makeArray(JSONAny))),
                                           },
                                           classTypes);

    auto ExecuteCommandRegistrationOptions = makeObject("ExecuteCommandRegistrationOptions",
                                                        {
                                                            makeField("commands", makeArray(JSONString)),
                                                        },
                                                        classTypes);

    auto ApplyWorkspaceEditParams = makeObject("ApplyWorkspaceEditParams",
                                               {
                                                   makeField("label", makeOptional(JSONString)),
                                                   makeField("edit", WorkspaceEdit),
                                               },
                                               classTypes);

    auto ApplyWorkspaceEditResponse = makeObject("ApplyWorkspaceEditResponse",
                                                 {
                                                     makeField("applied", JSONBool),
                                                 },
                                                 classTypes);

    auto DidOpenTextDocumentParams = makeObject("DidOpenTextDocumentParams",
                                                {
                                                    makeField("textDocument", TextDocumentItem),
                                                },
                                                classTypes);

    auto TextDocumentContentChangeEvent = makeObject("TextDocumentContentChangeEvent",
                                                     {
                                                         makeField("range", makeOptional(Range)),
                                                         makeField("rangeLength", makeOptional(JSONInt)),
                                                         makeField("text", JSONString),
                                                     },
                                                     classTypes);

    auto DidChangeTextDocumentParams =
        makeObject("DidChangeTextDocumentParams",
                   {
                       makeField("textDocument", VersionedTextDocumentIdentifier),
                       makeField("contentChanges", makeArray(TextDocumentContentChangeEvent)),
                   },
                   classTypes);

    auto TextDocumentChangeRegistrationOptions =
        makeObject("TextDocumentChangeRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("syncKind", TextDocumentSyncKind),
                   },
                   classTypes);

    auto TextDocumentSaveReason = makeIntEnum("TextDocumentSaveReason",
                                              {
                                                  {"Manual", 1},
                                                  {"AfterDelay", 2},
                                                  {"FocusOut", 3},
                                              },
                                              enumTypes);

    auto WillSaveTextDocumentParams = makeObject("WillSaveTextDocumentParams",
                                                 {
                                                     makeField("textDocument", TextDocumentIdentifier),
                                                     makeField("reason", TextDocumentSaveReason),
                                                 },
                                                 classTypes);

    auto DidSaveTextDocumentParams = makeObject("DidSaveTextDocumentParams",
                                                {
                                                    makeField("textDocument", TextDocumentIdentifier),
                                                    makeField("text", makeOptional(JSONString)),
                                                },
                                                classTypes);

    auto TextDocumentSaveRegistrationOptions =
        makeObject("TextDocumentSaveRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("includeText", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto DidCloseTextDocumentParams = makeObject("DidCloseTextDocumentParams",
                                                 {
                                                     makeField("textDocument", TextDocumentIdentifier),
                                                 },
                                                 classTypes);

    auto PublishDiagnosticsParams = makeObject("PublishDiagnosticsParams",
                                               {
                                                   makeField("uri", JSONString),
                                                   makeField("diagnostics", makeArray(Diagnostic)),
                                               },
                                               classTypes);

    auto CompletionTriggerKind = makeIntEnum("CompletionTriggerKind",
                                             {
                                                 {"Invoked", 1},
                                                 {"TriggerCharacter", 2},
                                                 {"TriggerForIncompleteCompletions", 3},
                                             },
                                             enumTypes);

    auto CompletionContext = makeObject("CompletionContext",
                                        {
                                            makeField("triggerKind", CompletionTriggerKind),
                                            makeField("triggerCharacter", makeOptional(JSONString)),
                                        },
                                        classTypes);

    auto CompletionParams = makeObject("CompletionParams",
                                       {
                                           makeField("textDocument", TextDocumentIdentifier),
                                           makeField("position", Position),
                                           makeField("context", makeOptional(CompletionContext)),
                                       },
                                       classTypes);

    auto InsertTextFormat = makeIntEnum("InsertTextFormat",
                                        {
                                            {"PlainText", 1},
                                            {"Snippet", 2},
                                        },
                                        enumTypes);

    auto CompletionItem =
        makeObject("CompletionItem",
                   {
                       makeField("label", JSONString),
                       makeField("kind", makeOptional(CompletionItemKind)),
                       makeField("detail", makeOptional(JSONString)),
                       makeField("documentation", makeOptional(makeVariant({JSONString, MarkupContent}))),
                       makeField("deprecated", makeOptional(JSONBool)),
                       makeField("preselect", makeOptional(JSONBool)),
                       makeField("sortText", makeOptional(JSONString)),
                       makeField("filterText", makeOptional(JSONString)),
                       makeField("insertText", makeOptional(JSONString)),
                       makeField("insertTextFormat", makeOptional(InsertTextFormat)),
                       makeField("textEdit", makeOptional(TextEdit)),
                       makeField("additionalTextEdits", makeOptional(makeArray(TextEdit))),
                       makeField("commitCharacters", makeOptional(makeArray(JSONString))),
                       makeField("command", makeOptional(Command)),
                       makeField("data", makeOptional(JSONAny)),
                   },
                   classTypes);

    auto CompletionRegistrationOptions =
        makeObject("CompletionRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("triggerCharacters", makeOptional(makeArray(JSONString))),
                       makeField("resolveProvider", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto CompletionList = makeObject("CompletionList",
                                     {
                                         makeField("isIncomplete", JSONBool),
                                         makeField("items", makeArray(CompletionItem)),
                                     },
                                     classTypes);

    auto Hover = makeObject(
        "Hover",
        {
            // Note: Can make this not a variant if we don't support deprecated version.
            // string | { language: string, value: string} | { language: string, value: string}[] | MarkupContent
            makeField("contents", JSONAny),
            makeField("range", makeOptional(Range)),
        },
        classTypes);

    auto ParameterInformation =
        makeObject("ParameterInformation",
                   {
                       makeField("label", JSONString),
                       makeField("documentation", makeOptional(makeVariant({JSONString, MarkupContent}))),
                   },
                   classTypes);

    auto SignatureInformation =
        makeObject("SignatureInformation",
                   {
                       makeField("label", JSONString),
                       makeField("documentation", makeOptional(makeVariant({JSONString, MarkupContent}))),
                       makeField("parameters", makeOptional(makeArray(ParameterInformation))),
                   },
                   classTypes);

    auto SignatureHelp = makeObject("SignatureHelp",
                                    {
                                        makeField("signatures", makeArray(SignatureInformation)),
                                        makeField("activeSignature", makeOptional(JSONInt)),
                                        makeField("activeParameter", makeOptional(JSONInt)),
                                    },
                                    classTypes);

    auto ReferenceContext = makeObject("ReferenceContext",
                                       {
                                           makeField("includeDeclaration", JSONBool),
                                       },
                                       classTypes);

    auto ReferenceParams = makeObject("ReferenceParams",
                                      {
                                          makeField("textDocument", TextDocumentIdentifier),
                                          makeField("position", Position),
                                          makeField("context", ReferenceContext),
                                      },
                                      classTypes);

    auto DocumentHighlightKind = makeIntEnum("DocumentHighlightKind",
                                             {
                                                 {"Text", 1},
                                                 {"Read", 2},
                                                 {"Write", 3},
                                             },
                                             enumTypes);

    auto DocumentHighlight = makeObject("DocumentHighlight",
                                        {
                                            makeField("range", Range),
                                            makeField("kind", makeOptional(DocumentHighlightKind)),
                                        },
                                        classTypes);

    auto DocumentSymbolParams = makeObject("DocumentSymbolParams",
                                           {
                                               makeField("textDocument", TextDocumentIdentifier),
                                           },
                                           classTypes);

    auto DocumentSymbol = makeObject("DocumentSymbol",
                                     {
                                         makeField("name", JSONString),
                                         makeField("detail", makeOptional(JSONString)),
                                         makeField("kind", DocumentHighlightKind),
                                         makeField("deprecated", makeOptional(JSONBool)),
                                         makeField("range", Range),
                                         makeField("selectionRange", Range),
                                     },
                                     classTypes);
    DocumentSymbol->addField(makeField("children", makeOptional(makeArray(DocumentSymbol))));

    auto SymbolInformation = makeObject("SymbolInformation",
                                        {
                                            makeField("name", JSONString),
                                            makeField("kind", SymbolKind),
                                            makeField("deprecated", makeOptional(JSONBool)),
                                            makeField("location", Location),
                                            makeField("containerName", makeOptional(JSONString)),
                                        },
                                        classTypes);

    auto CodeActionContext = makeObject("CodeActionContext",
                                        {
                                            makeField("diagnostics", makeArray(Diagnostic)),
                                            makeField("only", makeOptional(makeArray(CodeActionKind))),
                                        },
                                        classTypes);

    auto CodeActionParams = makeObject("CodeActionParams",
                                       {
                                           makeField("textDocument", TextDocumentIdentifier),
                                           makeField("range", Range),
                                           makeField("context", CodeActionContext),
                                       },
                                       classTypes);

    auto CodeAction = makeObject("CodeAction",
                                 {
                                     makeField("title", JSONString),
                                     makeField("kind", makeOptional(CodeActionKind)),
                                     makeField("diagnostics", makeOptional(makeArray(Diagnostic))),
                                     makeField("edit", makeOptional(WorkspaceEdit)),
                                     makeField("command", makeOptional(Command)),
                                 },
                                 classTypes);

    auto CodeActionRegistrationOptions =
        makeObject("CodeActionRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("codeActionKinds", makeOptional(makeArray(CodeActionKind))),
                   },
                   classTypes);

    auto CodeLensParams = makeObject("CodeLensParams",
                                     {
                                         makeField("textDocument", TextDocumentIdentifier),
                                     },
                                     classTypes);

    auto CodeLens = makeObject("CodeLens",
                               {
                                   makeField("range", Range),
                                   makeField("command", makeOptional(Command)),
                                   makeField("data", makeOptional(JSONAny)),
                               },
                               classTypes);

    auto DocumentLinkAndCodeLensRegistrationOptions =
        makeObject("CodeLensRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("resolveProvider", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto DocumentLinkParams = makeObject("DocumentLinkParams",
                                         {
                                             makeField("textDocument", TextDocumentIdentifier),
                                         },
                                         classTypes);

    auto DocumentLink = makeObject("DocumentLink",
                                   {
                                       makeField("range", Range),
                                       // URI
                                       makeField("target", makeOptional(JSONString)),
                                       makeField("data", makeOptional(JSONAny)),
                                   },
                                   classTypes);

    auto DocumentColorParams = makeObject("DocumentColorParams",
                                          {
                                              makeField("textDocument", TextDocumentIdentifier),
                                          },
                                          classTypes);

    auto Color = makeObject("Color",
                            {
                                makeField("red", JSONDouble),
                                makeField("green", JSONDouble),
                                makeField("blue", JSONDouble),
                                makeField("alpha", JSONDouble),
                            },
                            classTypes);

    auto ColorInformation = makeObject("ColorInformation",
                                       {
                                           makeField("range", Range),
                                           makeField("color", Color),
                                       },
                                       classTypes);

    auto ColorPresentationParams = makeObject("ColorPresentationParams",
                                              {
                                                  makeField("textDocument", TextDocumentIdentifier),
                                                  makeField("color", Color),
                                                  makeField("range", Range),
                                              },
                                              classTypes);

    auto ColorPresentation = makeObject("ColorPresentation",
                                        {
                                            makeField("label", JSONString),
                                            makeField("textEdit", makeOptional(TextEdit)),
                                            makeField("additionalTextEdits", makeOptional(makeArray(TextEdit))),
                                        },
                                        classTypes);

    auto DocumentFormattingParams = makeObject("DocumentFormattingParams",
                                               {
                                                   makeField("textDocument", TextDocumentIdentifier),
                                                   makeField("options", JSONAnyObject),
                                               },
                                               classTypes);

    auto DocumentRangeFormattingParams = makeObject("DocumentRangeFormattingParams",
                                                    {
                                                        makeField("textDocument", TextDocumentIdentifier),
                                                        makeField("range", Range),
                                                        makeField("options", JSONAnyObject),
                                                    },
                                                    classTypes);

    auto DocumentOnTypeFormattingParams = makeObject("DocumentOnTypeFormattingParams",
                                                     {
                                                         makeField("textDocument", TextDocumentIdentifier),
                                                         makeField("position", Position),
                                                         makeField("ch", JSONString),
                                                         makeField("options", JSONAnyObject),
                                                     },
                                                     classTypes);

    auto DocumentOnTypeFormattingRegistrationOptions =
        makeObject("DocumentOnTypeFormattingRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("firstTriggerCharacter", JSONString),
                       makeField("moreTriggerCharacter", makeOptional(JSONString)),
                   },
                   classTypes);

    auto RenameParams = makeObject("RenameParams",
                                   {
                                       makeField("textDocument", TextDocumentIdentifier),
                                       makeField("position", Position),
                                       makeField("newName", JSONString),
                                   },
                                   classTypes);

    auto RenameRegistrationOptions =
        makeObject("RenameRegistrationOptions",
                   {
                       makeField("documentSelector", makeVariant({DocumentSelector, JSONNull})),
                       makeField("prepareProvider", makeOptional(JSONBool)),
                   },
                   classTypes);

    auto FoldingRangeParams = makeObject("FoldingRangeParams",
                                         {
                                             makeField("textDocument", TextDocumentIdentifier),
                                         },
                                         classTypes);

    auto FoldingRangeKind = makeStrEnum("FoldingRangeKind", {"comment", "imports", "region"}, enumTypes);

    auto FoldingRange = makeObject("FoldingRange",
                                   {
                                       makeField("startLine", JSONInt),
                                       makeField("startCharacter", makeOptional(JSONInt)),
                                       makeField("endLine", JSONInt),
                                       makeField("endCharacter", makeOptional(JSONInt)),
                                       makeField("kind", makeOptional(FoldingRangeKind)),
                                   },
                                   classTypes);

    auto InitializeResult = makeObject("InitializeResult",
                                       {
                                           makeField("capabilities", ServerCapabilities),
                                       },
                                       classTypes);

    auto InitializeError = makeObject("InitializeError", {makeField("retry", JSONBool)}, classTypes);

    auto InitializeParams =
        makeObject("InitializeParams",
                   {
                       makeField("processId", makeVariant({JSONDouble, JSONNull})),
                       makeField("rootPath", makeVariant({JSONString, JSONNull})),
                       makeField("rootUri", makeVariant({JSONString, JSONNull})),
                       makeField("initializationOptions", makeOptional(JSONAny)),
                       makeField("capabilities", ClientCapabilities),
                       makeField("trace", makeOptional(TraceKind)),
                       makeField("workspaceFolders", makeOptional(makeVariant({JSONNull, makeArray(WorkspaceFolder)}))),
                   },
                   classTypes);
}
