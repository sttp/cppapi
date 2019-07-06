# STTP Data Sets

A [data set](https://github.com/sttp/cppapi/blob/master/src/lib/data/DataSet.h) represents an in-memory cache of records that is structured similarly to information defined in a database. The data set object consists of a collection of [data table](https://github.com/sttp/cppapi/blob/master/src/lib/data/DataTable.h) objects.

Data tables define of collection of [data columns](https://github.com/sttp/cppapi/blob/master/src/lib/data/DataColumn.h#L60) where each data column defines a name and [data type](https://github.com/sttp/cppapi/blob/master/src/lib/data/DataColumn.h#L32). Data columns can also be computed where its value would be derived from other columns and [functions](https://github.com/sttp/cppapi/blob/master/doc/FilterExpressions.md#filter-expression-functions) defined in an expression.

Data tables also define a set of [data rows](https://github.com/sttp/cppapi/blob/master/src/lib/data/DataRow.h) where each data row defines a record of information with a field value for each defined data column. Each field value can by `null` regardless of the defined data column type.

A data set schema and associated records can be read and written data from XML documents. The XML specification used for serialization is the standard for [W3C XML Schema Definition Language (XSD)](https://www.w3.org/TR/xmlschema/). See the [ReadXml and WriteXml](https://github.com/sttp/cppapi/blob/master/src/lib/data/DataSet.h#L80) functions.

> :information_source: The STTP data set functionality is modeled after, and generally interoperable with, the [.NET DataSet](https://docs.microsoft.com/en-us/dotnet/api/system.data.dataset). Serialized XML schemas and data saved from a .NET DataSet can be successfully parsed from an STTP data set and vice versa. Note that STTP requires that the schema be included with serialized XML data sets, see [XmlWriteMode.WriteSchema](https://docs.microsoft.com/en-us/dotnet/api/system.data.xmlwritemode). STTP does not attempt to infer a schema from the data. Interoperability with .NET XML schemas also includes [DataColumn expression](https://docs.microsoft.com/en-us/dotnet/api/system.data.datacolumn.expression) functionality, however, STTP defines more [functions](https://github.com/sttp/cppapi/blob/master/doc/FilterExpressions.md#filter-expression-functions) than the .NET implementation, so a serialized STTP data set that includes column expressions using functions not available to a .NET DataColumn will fail to evaluate when accessed from within .NET. Another difference from .NET is that the STTP implementation is always case-insensitive for table and column name lookups as the primary use-case for STTP data sets is for use with [filter expressions](https://github.com/sttp/cppapi/blob/master/doc/FilterExpressions.md).  